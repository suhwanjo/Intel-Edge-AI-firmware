/*
 * adc.h
 *
 *  Created on: Apr 9, 2024
 *      Author: IOT
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "main.h"
#include <stdbool.h>


// 버튼 이벤트 콜백 함수 타입
typedef void(*ADC_CBF)(void*);

// 버튼 객체 구조체
typedef struct{
	uint16_t value;
	ADC_CBF cbf;
} ADC_T;

#ifdef __cplusplus
extern "C"{
#endif

void adc_init(void);
void adc_regcbf(uint16_t idx, ADC_CBF cbf);
void adc_check(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_ADC_H_ */
