#include "cppmain.hpp"

#include "main.h"
#include "can.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include "stm32f3_easy_can.h"
#include "stm32f3_velocity.hpp"
#include "canmd_manager.h"
#include "stm32f3_antiphase_pwm.hpp"
#include "ps3.h"
#include "pid.hpp"

static int md_id = 0;
static uint8_t uart3_buf[8];

void setup(void) {
    // md_id初期化
    md_id = HAL_GPIO_ReadPin(DIP_SW_4_GPIO_Port, DIP_SW_4_Pin) << 3
          | HAL_GPIO_ReadPin(DIP_SW_3_GPIO_Port, DIP_SW_3_Pin) << 2
          | HAL_GPIO_ReadPin(DIP_SW_2_GPIO_Port, DIP_SW_2_Pin) << 1
          | HAL_GPIO_ReadPin(DIP_SW_1_GPIO_Port, DIP_SW_1_Pin) << 0;

    if(md_id == 0) {
        md_id = 0X7FF;
        HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET);
    }
    // ソフトウェアモジュール初期化
    canmd_manager_init();
    ps3_init();
    // ハードウェアモジュールスタート
    //// 通信関係
    stm32f3_easy_can_init(&hcan, md_id, 0X7FF);
    HAL_UART_Receive_IT(&huart3, uart3_buf, 8);
    //// PWM
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim8, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim8, TIM_CHANNEL_2);
    //// タイミングリソース
    HAL_TIM_Base_Start_IT(&htim6);
    HAL_TIM_Base_Start_IT(&htim7);
}

void loop(void) {
}

//**************************
//    タイマ割り込み関数
//**************************
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	// 5msecタイマ
	if(htim->Instance == TIM6) {
		int motor_control_data[2] = {};
        Ps3 ps3_data;
        int left_stick = 0;
		int right_stick = 0;

        // コントローラのデータ更新
		ps3_get_data(&ps3_data);
		// コントローラのstickの値更新(stickの値は- ～ 95の間で変化)
		if(ps3_data.left_analog_updown >= 64) {
		  left_stick = (ps3_data.left_analog_updown - 64) * 1023 / 63;
		}
		else {
		  left_stick = (ps3_data.left_analog_updown - 64) * 1023 / 64;
		}

		if(ps3_data.right_analog_updown >= 64) {
		  right_stick = (ps3_data.right_analog_updown - 64) * 1023 / 63;
		}
		else {
		  right_stick = (ps3_data.right_analog_updown - 64) * 1023 / 64;
		}

		// モータコントロールデータ取得
		motor_control_data[0] = left_stick/2;
        motor_control_data[1] = right_stick/2;

        // 速度計算
        static Stm32f3Velocity velocity[2] = {&htim2, &htim3};
        for(int i = 0; i < 2; i++){
            velocity[i].periodic_calculate_velocity();
        }

        //PID制御
        static Pid pid[2] = {{10,0,2},{1,0,0}};
        for(int i = 0; i < 2; i++){
            pid[i].get_enc(velocity[i].get_velocity());
            pid[i].get_ideal(motor_control_data[i]);
            pid[i].update_errors();
            motor_control_data[i] = pid[i].pid_cal();
        }

		// PWMのデューティー比計算
        double duty_rate[2];
        duty_rate[0] = motor_control_data[0] / (double)MOTOR_CONTROL_DATA_MAX; 
        duty_rate[1] = motor_control_data[1] / (double)MOTOR_CONTROL_DATA_MAX; 

		// PWMのデューティー比更新
        static Stm32f3AntiphasePwm pwm0(&htim8);
        static Stm32f3AntiphasePwm pwm1(&htim1);

        pwm0.update_duty(duty_rate[0]);
        pwm1.update_duty(duty_rate[1]);
	}

	// 約180msecタイマ
	if(htim->Instance == TIM7) {
		if(canmd_manager_time_out_check()) {
			HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_SET);
		}
		else {
			HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
		}
	}
}

//**************************
//    CAN通信受信割り込み
//**************************
void stm32f3_easy_can_interrupt_handler(void)
{
	int receive_id;
	int receive_dlc;
	unsigned char receive_message[8];

	// 受信データ取得
	stm32f3_easy_can_get_receive_message(&receive_id, &receive_dlc, receive_message);

	// 受信データ処理
    MdDataType receive_md_data_type
        = canmd_manager_set_can_receive_data(receive_message, receive_dlc);

    // 送信データ生成
    int transmit_id;
    int transmit_dlc;
    unsigned char transmit_message[8];

    // CAN通信の送信ID生成
    transmit_id = md_id   << 5 | 0b00000 ;
    //            送信元ID(5bit)  送信先ID(5bit)

    if(receive_md_data_type != MD_DATA_TYPE_CONTROL_DATA) {
        // 受信メッセージをそのまま送信メッセージとする
        transmit_dlc = receive_dlc;
        for(int i = 0; i < receive_dlc; i++) {
            transmit_message[i] = receive_message[i];
        }
    }
    else {
        // エンコーダのカウント値を送信メッセージとする
        static Stm32f3Velocity divided_encoder_count[2] = {&htim2, &htim3};
        for(int i = 0; i < 2; i++) {
            divided_encoder_count[i].periodic_calculate_velocity();
        }
        transmit_dlc = 3;
        transmit_message[0] = (MD_DATA_TYPE_CONTROL_DATA             << 6           )
                            | (divided_encoder_count[0].get_velocity() >> 5 & 0b111000)
                            | (divided_encoder_count[1].get_velocity() >> 8 & 0b111   );
        transmit_message[1] = divided_encoder_count[0].get_velocity() & 0XFF;
        transmit_message[2] = divided_encoder_count[1].get_velocity() & 0XFF;
    }

    // データ送信
    stm32f3_easy_can_transmit_message(transmit_id, transmit_dlc, transmit_message);

	return;
}

//*********************************************************
//    UART割り込み関数
//*********************************************************
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == huart3.Instance) {
		ps3_uart_interrupt_routine(uart3_buf, 8);
		HAL_UART_Receive_IT(&huart3, uart3_buf, 8);
	}
}