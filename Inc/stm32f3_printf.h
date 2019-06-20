/*
 * stm32f3_printf.h
 *  Vertion   : 01.00
 *  Created on: 2018/12/15
 *      Author: hajime
 */

#ifndef UARTPRINTF_H_
#define UARTPRINTF_H_

#include "stm32f3xx_hal.h"
#include <stdint.h>

void stm32f3_printf_init(UART_HandleTypeDef *huart);
void stm32f3_puts(uint8_t USART_TX_data[]);
void stm32f3_printf(const char *format, ...);

#endif /* UARTPRINTF_H_ */
