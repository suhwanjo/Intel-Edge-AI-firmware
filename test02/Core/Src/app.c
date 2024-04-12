/*
 * app.c
 *
 *  Created on: Apr 11, 2024
 *      Author: IOT
 */
#include <stdio.h>
#include "main.h"
#include "polling.h"
#include "button.h"
#include "adc.h"
#include "io.h"
#include "uart.h"

void app_init(void);


void app(void) {
    // Boot message
    printf("booting\r\n");

	app_init();

    // Enter the main loop
    while (1) {
		button_thread(NULL);
		polling_thread(NULL);
		uart_thread(NULL);
		// Do nothing, the button and ADC events are handled by the interrupt handlers
    }
}

void app_init(void)
{
	io_exti_init();
	polling_init();
	button_init();
	uart_init();
}

