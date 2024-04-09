/*
 * io.h
 *
 *  Created on: Mar 29, 2024
 *      Author: IOT
 */
#include <stdbool.h>
#ifndef SRC_IO_H_
#define SRC_IO_H_

#ifdef __cplusplus
extern "C"{
#endif

int __io_putchar(int ch);
void led_onoff(bool on_flag, uint8_t led_num);
void led_onoff2(bool on_flag, uint8_t led_num);

#ifdef __cplusplus
}
#endif


#endif /* SRC_IO_H_ */
