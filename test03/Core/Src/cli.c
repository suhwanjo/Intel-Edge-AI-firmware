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
#include "uart.h"
#include "app.h"
#include "tim.h"
#include "led.h"
#include "cli.h"

typedef struct {
	char *cmd;	// 명령어
	uint8_t no;	// 인자 최소 갯수
	int (*cbf)(int, char **);  // int argc, char *argv[]
	char *remark; // 설명
} CMD_LIST_T;

static osThreadId_t cli_thread_hnd;	// 쓰레드 핸들
static osEventFlagsId_t cli_evt_id;		// 이벤트 플래그 핸들

static const osThreadAttr_t cli_thread_attr = {
  .stack_size = 128 * 8,
  .priority = (osPriority_t) osPriorityNormal,
};

static int cli_led(int argc, char **argv);
static int cli_echo(int argc, char *argv[]);
static int cli_help(int argc, char *argv[]);
static int cli_mode(int argc, char *argv[]);
static int cli_dump(int argc, char *argv[]);
static int cli_duty(int argc, char *argv[]);

const CMD_LIST_T gCmdListObjs[] = {
	{ "duty",		2,		cli_duty,		"led 1 pwm duty\r\n duty [duty:0~999]"	},
	{ "dump",	3,		cli_dump,	"memory dump\r\n dump [address:hex] [length:max:10 lines]"	},
	{ "mode",	2,		cli_mode,	"change application mode\r\n mode [0/1]"	},
	{ "led",		3,		cli_led,		"led [1/2/3] [on/off]"	},
	{ "echo",		2,		cli_echo,		"echo [echo data]"	},
	{ "help", 		1, 		cli_help, 		"help" 					},
	{ NULL,		0,		NULL,			NULL						}
};
static int cli_duty(int argc, char *argv[])
{
	uint16_t duty;

	if (argc < 2) {
		printf("Err : Arg No\r\n");
		return -1;
	}

	duty = (uint16_t)strtol(argv[1], NULL, 10);
	printf("tim_duty_set(%d)\r\n", duty);
	//tim_duty_set(duty);

	return 0;
}

static int cli_dump(int argc, char *argv[])
{
	uint32_t address, length, temp;

	if (argc < 3) {
		printf("Err : Arg No\r\n");
		return -1;
	}

	if (strncmp(argv[1], "0x", 2) == 0) address = (uint32_t)strtol(&argv[1][2], NULL, 16);
	else address = (uint32_t)strtol(&argv[1][0], NULL, 16);

	length = (uint32_t)strtol(argv[2], NULL, 10);
	if (length > 10) length = 10;

	printf("address  %08lX, length = %ld\r\n", address, length);

	for (int i=0; i<length; i++) {
		printf("\r\n%08lX : ", (uint32_t)address);

		temp=address;
		for (int j=0; j<16; j++) {
			printf("%02X ", *(uint8_t *)temp);
			temp++;
		}

		temp=address;
		for (int j=0; j<16; j++) {
			char c = *(char *)temp;
			c = isalnum(c) ? c : (char)' ';
			printf("%c", c);
			temp++;
		}

		address = temp;
	}
	printf("\r\n");

	return 0;
}

static int cli_mode(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Err : Arg No\r\n");
		return -1;
	}

	long mode = strtol(argv[1], NULL, 10);
	//app_mode((int)mode);

	return 0;
}

static int cli_led(int argc, char *argv[])
{
	if (argc < 3) {
		printf("Err : Arg No\r\n");
		return -1;
	}

	long no = strtol(argv[1], NULL, 10);
	int onoff = strcmp(argv[2], "off");

	if (onoff != 0) onoff = 1;
	bool sts = onoff ? true : false;

	printf("led %ld, %d\r\n", no, onoff);
	 //led_onoff((uint8_t)no, sts);

	return 0;
}

static int cli_echo(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Err : Arg No\r\n");
		return -1;
	}
	printf("%s\r\n", argv[1]);

	return 0;
}

static int cli_help(int argc, char *argv[])
{
	for (int i=0; gCmdListObjs[i].cmd != NULL; i++) {
		printf("%s\r\n", gCmdListObjs[i].remark);
	}

	return 0;
}

static void cli_parser(BUF_T *arg);
static void cli_event_set(void *arg);

static BUF_T gBufObj[1];

void cli_thread(void *arg)
{
	uint32_t flags;
	(void)arg;

	printf("CLI Thread Started...\r\n");

	uart_regcbf(cli_event_set);

	while (1) {
		flags = osEventFlagsWait(cli_evt_id, 0xffff, osFlagsWaitAny, osWaitForever);

		if (flags & 0x0001) cli_parser(&gBufObj[0]);
	}
}

void cli_init(void)
{
	cli_evt_id = osEventFlagsNew(NULL);
	if (cli_evt_id != NULL) printf("CLI Event Flags Created...\r\n");
	else {
		printf("CLI Event Flags Create File...\r\n");
		while (1);
	}

	cli_thread_hnd = osThreadNew(cli_thread, NULL, &cli_thread_attr);
	if (cli_thread_hnd != NULL) printf("CLI Thread Created...\r\n");
	else {
		printf("CLI Thread Create Fail...\r\n");
		while (1);
	}
}

static void cli_event_set(void *arg)
{
	BUF_T *pBuf = (BUF_T *)arg;

	memcpy(&gBufObj[0], pBuf, sizeof(BUF_T));

	osEventFlagsSet(cli_evt_id, 0x0001);
}


#define D_DELIMITER		" ,\r\n"

static void cli_parser(BUF_T *arg)
{
	int argc = 0;
	char *argv[10];
	char *ptr;

	char *buf = (char *)arg->buf;

	//printf("rx:%s\r\n", (char *)arg);
	// token 분리
	ptr = strtok(buf, D_DELIMITER);
	if (ptr == NULL) return;

	while (ptr != NULL) {
		argv[argc] = ptr;
		argc++;
		ptr = strtok(NULL, D_DELIMITER);
	}

//	for (int i=0; i<argc; i++) {
//		printf("%d:%s\r\n", i, argv[i]);
//	}

	for (int i=0; gCmdListObjs[i].cmd != NULL; i++) {
		if (strcmp(gCmdListObjs[i].cmd, argv[0]) == 0) {
			gCmdListObjs[i].cbf(argc, argv);
			return;
		}
	}

	printf("Unsupported Command\r\n");
}
