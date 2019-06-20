/* 
 * File:    stm32f3_easy_can.h
 * version: ver02.00 (2019/04/01)
 * Author:  hajime
 *
 * Created on 2019/03/26, 21:42
 */

#ifndef STM32F3_EASY_CAN_H_
#define STM32F3_EASY_CAN_H_

#include "stm32f3xx_hal.h"

void stm32f3_easy_can_init(CAN_HandleTypeDef *hcan, int acceptance_filter, int filter_mask);
void stm32f3_easy_can_get_receive_message(int *receive_id, int *receive_dlc, unsigned char receive_message[]);
int stm32f3_easy_can_transmit_message(int transmit_id, int transmit_dlc, const unsigned char transmit_message[]);
void stm32f3_easy_can_interrupt_handler(void); // 受信割り込みハンドラ(受信割り込みが発生すると呼ばれる．自分で定義する必要がある)

#endif /* STM32F3_EASY_CAN_H_ */
