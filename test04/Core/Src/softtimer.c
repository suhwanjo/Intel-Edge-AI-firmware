/*
 * softtimer.c
 *
 *  Created on: Apr 18, 2024
 *      Author: IOT
 */
#include "cmsis_os.h"
#include <stdio.h>

static osTimerId_t timer_hnd;

static void periodic_func(void *);
void (*timer_cbf)(void);

void softtimer_init(void) // init, start, stop
{
	timer_hnd = osTimerNew(periodic_func, osTimerPeriodic, NULL, NULL);
	if(timer_hnd != NULL){
		printf("Soft Timer Created..\r\n");
	}
	else{
		printf("Soft Timer Created Fail..\r\n");
		//__assert__(__func, __LINE__);
		while(1);
	}
}

void softtimer_start(uint32_t ticks)
{
	osTimerStart(timer_hnd, ticks);
}

void softtimer_stop(void)
{
	osTimerStop(timer_hnd);
}

void softtimer_regcbf(void (*cbf)(void))
{
	timer_cbf = cbf;
}

static void periodic_func(void *arg)
{
	(void)arg;

	if(timer_cbf != NULL) timer_cbf();
}
