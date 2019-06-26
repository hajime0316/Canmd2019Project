#include "cppmain.hpp"

#include "main.h"
#include "can.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include "stm32f3_easy_can.h"
#include "stm32f3_velocity.hpp"
#include "canmd_manager.h"
#include "stm32f3_printf.h"

static int md_id = 0;

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
    // ハードウェアモジュールスタート
    //// 通信関係
    stm32f3_printf_init(&huart3);
    stm32f3_easy_can_init(&hcan, md_id, 0X7FF);
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
    int motor_control_data[2];
    // モーターコントロールデータ取得
    canmd_manager_get_motor_control_data(motor_control_data);

    // デバッグ出力
    stm32f3_printf("%5d  %5d  ", motor_control_data[0], motor_control_data[1]);
    stm32f3_printf("%3d", md_id);
    stm32f3_printf("\r\n");
}

//**************************
//    タイマ割り込み関数
//**************************
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	// 5msecタイマ
	if(htim->Instance == TIM6) {
		int motor_control_data[2] = {};

		// モータコントロールデータ取得
		canmd_manager_get_motor_control_data(motor_control_data);

		// PWMのデューティー比更新
		TIM8->CCR1 = PWM_DUTY_ZERO + PWM_DUTY_MAX * motor_control_data[0] / (double)MOTOR_CONTROL_DATA_MAX;
		TIM8->CCR2 = PWM_DUTY_ZERO - PWM_DUTY_MAX * motor_control_data[0] / (double)MOTOR_CONTROL_DATA_MAX;
		TIM1->CCR1 = PWM_DUTY_ZERO + PWM_DUTY_MAX * motor_control_data[1] / (double)MOTOR_CONTROL_DATA_MAX;
		TIM1->CCR2 = PWM_DUTY_ZERO - PWM_DUTY_MAX * motor_control_data[1] / (double)MOTOR_CONTROL_DATA_MAX;
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

    if(receive_md_data_type == MD_DATA_TYPE_CONTROL_DATA) {
        // エンコーダのカウント値を送信メッセージとする
        static Stm32f3Velocity divided_encoder_count[2] = {&htim2, &htim3};
        for(int i = 0; i < 2; i++) {
            divided_encoder_count[i].periodic_calculate_velocity();
        }
        transmit_dlc = 3;
        transmit_message[0] = (MD_DATA_TYPE_CONTROL_DATA             << 6           )
                            | (divided_encoder_count[0].get_velocity >> 5 & 0b111000)
                            | (divided_encoder_count[1].get_velocity >> 8 & 0b111   );
        transmit_message[1] = divided_encoder_count[0].get_velocity & 0XFF;
        transmit_message[2] = divided_encoder_count[1].get_velocity & 0XFF;
    }
    else {
        // 受信メッセージをそのまま送信メッセージとする
        transmit_dlc = receive_dlc;
        for(int i = 0; i < receive_dlc; i++) {
            transmit_message[i] = receive_message[i];
        }
    }

    // データ送信
    stm32f3_easy_can_transmit_message(transmit_id, transmit_dlc, transmit_message);

	return;
}
