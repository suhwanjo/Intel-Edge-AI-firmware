/*
 * io.c
 *
 *  Created on: Mar 29, 2024
 *      Author: IOT
 */
#include "main.h" // UART_HandleTypeDef가 main.h에 정의
#include "io.h"
#include <stdbool.h>
// main.c에 정의, main에 있는 변수이니 extern 사용
extern UART_HandleTypeDef huart3;
#define D_PRINTF_UART_HANDLE huart3

int __io_putchar(int ch) // syscall.c에 정의
{
	HAL_UART_Transmit(&D_PRINTF_UART_HANDLE, (uint8_t *)&ch, 1, 0xffff); // 정의 찾아서 채워넣기
	return ch;
}
/// 구조화 ///
// GPIO_TypeDef *gLedObjs로 선언해서 gLedObjs->BSRR로 쓰는 것과 같음
#define PORTB_BSRR_BASE_ADDR	0x40020418
#define LD1_BIT					0
#define LD2_BIT 				7
#define LD3_BIT					14
#define MAX_LED					3

typedef struct{
	uint32_t* bsrr_addr;
	uint16_t on;
	uint16_t off;
} LED_T;

const LED_T gLedObjs[MAX_LED]={
		{(uint32_t*)PORTB_BSRR_BASE_ADDR, LD1_BIT, 16 + LD1_BIT},
		{(uint32_t*)PORTB_BSRR_BASE_ADDR, LD2_BIT, 16 + LD2_BIT},
		{(uint32_t*)PORTB_BSRR_BASE_ADDR, LD3_BIT, 16 + LD3_BIT}
};
/// 구조화 ///


void led_onoff(bool on_flag, uint8_t led_num)
{
	if(led_num >= MAX_LED)
		return;
	*gLedObjs[led_num].bsrr_addr = on_flag ? (0x1 << gLedObjs[led_num].on) : (0x1 << gLedObjs[led_num].off);
	//if(pin_num == LD1_BIT){
	//	uint32_t bsrr_addr = PORTB_BSRR_BASE_ADDR;
	//	uint32_t bit = on_flag ? (0x1 << LD1_BIT) : (0x1 << (16 + LD1_BIT));
	//	*(uint32_t*)bsrr_addr = bit;
	//}
	//else if(pin_num == LD2_BIT){
	//	uint32_t bsrr_addr = PORTB_BSRR_BASE_ADDR;
	//	uint32_t bit = on_flag ? (0x1 << LD2_BIT) : (0x1 << (16 + LD2_BIT));
	//	*(uint32_t*)bsrr_addr = bit;
	//}
	//else if(pin_num == LD3_BIT){
	//	uint32_t bsrr_addr = PORTB_BSRR_BASE_ADDR;
	//	uint32_t bit = on_flag ? (0x1 << LD3_BIT) : (0x1 << (16 + LD3_BIT));
	//	*(uint32_t*)bsrr_addr = bit;
	//}
	//else
	//	return;
	// 0x00001 == 0x1 << 0;
	//*(uint32_t*)(PORTB_BASE + ODR_OFFSET) |= (0x1 << 0);
	//*(uint32_t*)(PORTB_BASE + ODR_OFFSET) |= (0x1 << 7);
	//*(uint32_t*)(PORTB_BASE + ODR_OFFSET) |= (0x1 << 14);
}
void led_onoff2(bool on_flag, uint8_t led_num){
	if(led_num >= MAX_LED)
			return;
	LED_T *p;
	p = (LED_T *)&gLedObjs[led_num];
	*p->bsrr_addr = on_flag ? (0x1 << p->on) : (0x1 << p->off);
}
