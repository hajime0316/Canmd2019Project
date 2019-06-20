/*
 * stm32f3_printf.c
 *  Vertion   : 01.00
 *  Created on: 2018/12/15
 *      Author: hajime
 */

#include "stm32f3xx_hal.h"
#include "stm32f3_printf.h"

#include <stdarg.h>
#include <stdlib.h>

static UART_HandleTypeDef *huart_printf;

void stm32f3_printf_init(UART_HandleTypeDef *huart)
{
	huart_printf = huart;
}

void stm32f3_puts(uint8_t USART_TX_data[])
{
	int i;

	for(i=0;USART_TX_data[i]!='\0';i++){
		HAL_UART_Transmit(huart_printf, &USART_TX_data[i], sizeof(USART_TX_data[i]), 0x1000);
	}
}

void stm32f3_printf(const char *format, ...)
{
	va_list list;

	va_start(list, format);

	int len = vsnprintf(0, 0, format, list);
	char *s;

	s = (char *)malloc(len + 1);
	vsprintf(s, format, list);

	stm32f3_puts((uint8_t *)s);

	free(s);

	va_end(list);

	return;
}
