/*
 * uart.h
 *
 *  Created on: Apr 11, 2024
 *      Author: IOT
 */

#ifndef INC_UART_H_
#define INC_UART_H_

#include "main.h"

#ifdef __cplusplus
extern "C"{
#endif

void uart_init(void);
void uart_thread(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* INC_UART_H_ */
