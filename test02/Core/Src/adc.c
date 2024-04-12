/*
 * adc.c
 *
 *  Created on: Apr 11, 2024
 *      Author: IOT
 */

#include <stdio.h>
#include "adc.h"

static void adc_dummy(void *); // 경고 없애기 위해

static ADC_T gAdcObjs[] = {
		{.cbf = adc_dummy},
		{.cbf = NULL	 }
};

void adc_regcbf(uint16_t idx, ADC_CBF cbf)
{
	gAdcObjs[idx].cbf = cbf;
}

void adc_thread(void *arg)
{
	(void)arg;

	static uint16_t value = 100;

	for (int i=0; gAdcObjs[i].cbf != NULL; i++) {
		value+=100;
		gAdcObjs[i].value = value;
		gAdcObjs[i].cbf((void *)&gAdcObjs[i].value);
	}
}

// 빈 콜백 함수
static void adc_dummy(void *)
{
    return;
}
void adc_init(void)
{
}
