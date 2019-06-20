/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f3_easy_can.h"
#include "canmd_manager.h"
#include "stm32f3_printf.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
static int md_id = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  int motor_control_data[2];
  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_TIM8_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_USART3_UART_Init();
  MX_CAN_Init();
  /* USER CODE BEGIN 2 */
  // md_id初期化
  md_id = HAL_GPIO_ReadPin(DIP_SW_4_GPIO_Port, DIP_SW_4_Pin) << 3
  			| HAL_GPIO_ReadPin(DIP_SW_3_GPIO_Port, DIP_SW_3_Pin) << 2
  			| HAL_GPIO_ReadPin(DIP_SW_2_GPIO_Port, DIP_SW_2_Pin) << 1
  			| HAL_GPIO_ReadPin(DIP_SW_1_GPIO_Port, DIP_SW_1_Pin) << 0;
  if(md_id == 0) {
	  md_id = 0X7FF;
	  HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, RESET);
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

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  // モーターコントロールデータ取得
	  canmd_manager_get_motor_control_data(motor_control_data);

	  // デバッグ出力
	  stm32f3_printf("%5d  %5d  ", motor_control_data[0], motor_control_data[1]);
	  stm32f3_printf("%3d", md_id);
	  stm32f3_printf("\r\n");

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART3|RCC_PERIPHCLK_TIM1
                              |RCC_PERIPHCLK_TIM8;
  PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLK_HCLK;
  PeriphClkInit.Tim8ClockSelection = RCC_TIM8CLK_HCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
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
			HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, SET);
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
	canmd_manager_set_can_receive_data(receive_message, receive_dlc);

	return;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
