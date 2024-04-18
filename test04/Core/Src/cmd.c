/*
 * cmd.c
 *
 *  Created on: Apr 17, 2024
 *      Author: iot00
 */

/*
 * cli.c
 *
 *  Created on: Apr 12, 2024
 *      Author: iot00
 */

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include "cmsis_os.h"
#include "type.h"
#include "uart.h"
#include "app.h"
#include "cmd.h"
#include "mem.h"
#include "cli.h"
#include "i2c_HD44780.h" // 추가
#include "softtimer.h"

static osThreadId_t cmd_thread_hnd;	// 쓰레드 핸들
static osMessageQueueId_t cmd_msg_id;  //메시지큐 핸들

static const osThreadAttr_t cmd_thread_attr = {
  .stack_size = 128 * 8,
  .priority = (osPriority_t) osPriorityNormal,
};

static void cmd_msg_put_0(void *arg); // 추가
static void cmd_msg_put(void *arg);

void lcd_test(void)
{
	lcd_init();   // initialize lcd
	lcd_disp_on();
	lcd_clear_display();
	lcd_home();
	//lcd_printf("hello");
	//lcd_locate(2,0);
	//lcd_printf("World");
}

void softtimer_callback(void)
{
	static uint8_t count = 0;
	printf("cnt = %d\r\n", count++);
	static uint8_t state =0;

	lcd_locate(2,10);
	switch(state){
		case 0:{
			lcd_printchar('.'); state++;
		}break;
		case 1:{
			lcd_printchar('o'); state++;
		}break;
		case 2:{
			lcd_printchar('O'); state++;
		}break;
		case 3:{
			lcd_printchar('@'); state = 0;
		}break;
	}
}

void cmd_thread(void *arg)
{
	(void)arg;
	osStatus sts;
	MSG_T rxMsg; //, txMsg;

	uart_regcbf(E_UART_0, cmd_msg_put_0);
	uart_regcbf(E_UART_1, cmd_msg_put);

	lcd_test();

	//softtimer_init();
	//softtimer_regcbf(softtimer_callback);
	//softtimer_start(500);

	while (1) {
		sts = osMessageQueueGet(cmd_msg_id, &rxMsg, NULL, osWaitForever);

		if (sts == osOK) {
			switch (rxMsg.id) {
				case E_MSG_CMD_RX_0 : {
						MEM_T *pMem = (MEM_T *)rxMsg.body.vPtr;
						PKT_T *pRxPkt = (PKT_T *)pMem->buf;

						printf("E_MSG_CMD_RX_0\r\n");
						printf("cmd=%04x, len=%d ", pRxPkt->cmd, pRxPkt->len);
						for (int i=0; i<pRxPkt->len; i++) {
							printf("%02x ", pRxPkt->ctx[i]);
						}
						printf("\r\n");

						mem_free(pMem);
					} break;

				case E_MSG_CMD_RX : {
					MEM_T *pMem = (MEM_T *)rxMsg.body.vPtr;
					PKT_T *pRxPkt = (PKT_T *)pMem->buf;

					printf("E_MSG_CMD_RX\r\n");

					switch (pRxPkt->cmd) {
						case E_CMD_LED : { // 00 00
							printf("LED command\r\n");
							printf("cmd=%04x, len=%d ", pRxPkt->cmd, pRxPkt->len);
							for (int i=0; i<pRxPkt->len; i++) {
								printf("%02x ", pRxPkt->ctx[i]);
							}
							printf("\r\n");

							{
								bool res = false;
								uint16_t enc_len;
								uint8_t enc_data[200];
								res = slip_encode(pMem->buf, pRxPkt->len + 3, enc_data, &enc_len);
								if (res == true) {
									for (uint16_t i=0; i<enc_len; i++) {
										printf("%02x ", enc_data[i]);
									}
									printf("\r\n");
								}
								extern UART_HandleTypeDef huart2;
								HAL_UART_Transmit(&huart2, enc_data, enc_len, 0xffff);
							}

							mem_free(pMem); //					}break;
						} break;

						case E_CMD_CLI:{ // 01 00
							// PKT_T 구조체 사용한 withrobot 디코딩
							printf("CLI Command\r\n");
							pRxPkt->ctx[pRxPkt->len] = 0; // 끝에 '\0'(널 문자) 달아서 cli_msg_put으로
							cli_msg_put((void *)pMem);
						} break;

						// 방법 1
						case E_CMD_LCD: { // LCD 출력 명령 처리
							printf("LCD Command\r\n");
							pRxPkt->ctx[pRxPkt->len] = 0; // '\0'
							cli_msg_put((void *)pMem);
						} break;
// 방법 2
//#define print_lcd(r, c)  \
//do { \
//	lcd_locate(r, c); \
//	pRxPkt->ctx[pRxPkt->len] = 0; \
//	char *str = (char *)pRxPkt->ctx; \
//	lcd_print_string(str); \
//} while(0)
//						case E_CMD_LCD_LINE_1: {
//							print_lcd(1, 1);
//							mem_free(pMem);
//						} break;
//
//						case E_CMD_LCD_LINE_2: {
//							print_lcd(2, 1);
//							mem_free(pMem);
//						} break;
					}
//				printf("cmd=%04x, len=%d",pRxPkt->cmd,pRxPkt->len);
//				for(int i=0; i < pRxPkt->len; i++){
//					printf("%02x ",pRxPkt->ctx[i]);
//				}
//				printf("\r\n");
//				mem_free(pMem); //pMem으로 바꾸면서 free하면 안됨
			}break;
			}
		}
	}
}

void cmd_init(void)
{
	cmd_msg_id = osMessageQueueNew(3, sizeof(MSG_T), NULL);

	if (cmd_msg_id != NULL) printf("CMD Message Queue Created...\r\n");
	else {
		printf("CMD Message Queue  Create File...\r\n");
		while (1);
	}

	cmd_thread_hnd = osThreadNew(cmd_thread, NULL, &cmd_thread_attr);
	if (cmd_thread_hnd != NULL) printf("CMD Thread Created...\r\n");
	else {
		printf("CMD Thread Create Fail...\r\n");
		while (1);
	}
}

// ISR
static void cmd_msg_put_0(void *arg)
{
	BUF_T *pBuf = (BUF_T *)arg;

	MEM_T *pMem = mem_alloc(100, 0);
	if (pMem != NULL) {
		memcpy(pMem->buf, pBuf, pBuf->idx);
		Q_PUT(cmd_msg_id, E_MSG_CMD_RX_0, pMem, 0);
	}
}
static void cmd_msg_put(void *arg)
{
	BUF_T *pBuf = (BUF_T *)arg;

	MEM_T *pMem = mem_alloc(100,0); // timeout=0
	if(pMem != NULL){
		memcpy(pMem->buf, pBuf, pBuf->idx); // pMem->buf는 주소
		Q_PUT(cmd_msg_id, E_MSG_CMD_RX, pMem, 0);
	}
//	memcpy(&gBufObj[0], pBuf, sizeof(BUF_T));
//
//	MSG_T txMsg;
//	txMsg.id = E_MSG_CLI;
//	txMsg.body.vPtr = (void *)&gBufObj[0];
//	osMessageQueuePut(cmd_msg_id, &txMsg, 0, 0);
}
