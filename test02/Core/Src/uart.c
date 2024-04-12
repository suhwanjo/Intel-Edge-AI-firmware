/*
 * uart.c
 *
 *  Created on: Apr 11, 2024
 *      Author: IOT
 */

#include "uart.h"
#include <stdbool.h>
#include <stdio.h>

extern UART_HandleTypeDef huart3;

static uint8_t rxd;
static bool rxd_flag = false;

#define D_BUF_MAX 100

void uart_init(void)
{
	HAL_UART_Receive_IT(&huart3, (uint8_t *)&rxd, 1);
}

void uart_thread(void *arg)
{
	if(rxd_flag == true){
		rxd_flag = false;
		printf("rxd: %c \r\n",rxd);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	rxd_flag = true;
	HAL_UART_Receive_IT(&huart3, (uint8_t *)&rxd, 1);

}
