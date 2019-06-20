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

#ifdef __cplusplus
extern "C" {
#endif

void stm32f3_printf_init(UART_HandleTypeDef *huart);
void stm32f3_puts(uint8_t USART_TX_data[]);
void stm32f3_printf(const char *format, ...);

#ifdef __cplusplus
};
#endif

#endif /* UARTPRINTF_H_ */
