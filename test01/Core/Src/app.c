/*
 * app.c
 *
 *  Created on: Apr 8, 2024
 *      Author: IOT
 */
#include <stdio.h>
#include "main.h"
#include <stdbool.h>
#include "button.h"
#include "adc.h"

void button_callback2(void *arg);
void button_callback(void *arg);
void adc_callback(void *arg);

/*bool bsp_read_btn1(void)
{
	return HAL_GPIO_ReadPin(USER_Btn_GPIO_Port, USER_Btn_Pin) == GPIO_PIN_SET ? true : false;
}*/

// 이 밑은 수정하지 않도록 해야 함
//#define bsp_delay(x)		HAL_Delay(x)
/*void app(void)
{
	bool sts;
	//uint16_t btn;
	//GPIO_TypeDef *pPort = GPIOC; //(GPIO_TypeDef *)0x40020800; // PC base addr
	//GPIO_PinState sts;
	printf("system start...\r\n");

	while(1){
		// hard coding
		//btn = *(uint16_t*)0x40020810; // port C IDR
		//btn = pPort->IDR;

		//if(btn & (0x1 << 13)){
		//if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)){
		bsp_delay(200);

		sts = bsp_read_btn1();
		if(sts == true){
			printf("Blue Button Pushed\r\n");
		}
	}
}*/
// 버튼 콜백 함수 2
void button_callback2(void *arg)
{
    // 정적 카운터 변수
    static uint8_t count = 0;

    // 버튼 상태 정보 구조체 포인터
    BUTTON_STS *pSts = (BUTTON_STS *)arg;

    // 버튼이 눌렸을 때(Rising)
    if (pSts->edge == true)
        printf("Rising!\r\n");
    // 버튼이 떼어졌을 때(Falling)
    else if (pSts->edge == false)
        printf("Falling! : period = %d\r\n", pSts->pushed_count);

    // 카운터 값 출력
    printf("2. count = %d\r\n", count);

    // 카운터 증가
    count++;
    count %= 10;

    // 카운터가 10이 되면
    if (count == 10) {
        // 콜백 함수를 button_callback으로 변경
        button_regcbf(0, button_callback);
        printf("cbf changed to callback2!\r\n");
    }
}
void adc_callback(void *arg)
{
	printf("adc value = %d\r\n",*(uint16_t *)arg);
}
// 버튼 콜백 함수 1
void button_callback(void *arg)
{
    // 정적 카운터 변수
    static uint8_t count = 0;

    // 버튼 상태 정보 구조체 포인터
    BUTTON_STS *pSts = (BUTTON_STS *)arg;

    // 버튼이 눌렸을 때(Rising)
    if (pSts->edge == true)
        printf("Rising!\r\n");
    // 버튼이 떼어졌을 때(Falling)
    else if (pSts->edge == false)
        printf("Falling! : period = %d\r\n", pSts->pushed_count);

    // 카운터 값 출력
    printf("1. count = %d\r\n", count);

    // 카운터 증가
    count++;
    count %= 10;

    // 카운터가 10이 되면
    if (count == 10) {
        // 콜백 함수를 button_callback2로 변경
        button_regcbf(0, button_callback2);
        printf("cbf changed to callback!\r\n");
    }
}
// 스레드 구조체
typedef struct{
	uint32_t period, count;
	bool flag;
	void (*cbf)(void);
}THR_T;

THR_T gThrObjs[] = {
    {.period = 1,   .count = 0, .flag = false, .cbf = button_check},
	{.period = 500, .count = 0, .flag = false, .cbf = adc_check   },
    {.period = 0,   .count = 0, .flag = false, .cbf = NULL		  }
};

static void init(void)
{
	// 버튼 드라이버 초기화
	button_init();
	// 버튼 콜백 함수 등록 (button_callback)
	button_regcbf(0, button_callback);

	adc_init();
	adc_regcbf(0, adc_callback);
}

// 애플리케이션 메인 함수
void app(void)
{
	uint32_t thr_idx = 0;
    // 시간 측정 변수
    uint32_t tick_prev, tick_curr;

    // 부팅 메시지 출력
    printf("booting\r\n");
    init();

    // 시간 측정 변수 초기화
    tick_prev = tick_curr = HAL_GetTick();

    // 무한 루프
    while (1) { // 폴링 방식 ISR
        // 현재 시간 측정
        tick_curr = HAL_GetTick();
    	if (tick_curr - tick_prev >= 1){
			for(int i = 0; gThrObjs[i].cbf != NULL; i++){
				gThrObjs[i].count++;
				gThrObjs[i].count %= gThrObjs[i].period;
				if(gThrObjs[i].count == 0) gThrObjs[i].flag = true;
			}
			// 이전 시간 업데이트
			tick_prev = tick_curr;
        }
        {
        	if(gThrObjs[thr_idx].flag == true){
        		gThrObjs[thr_idx].flag = false;
        		gThrObjs[thr_idx].cbf(); // 콜백 함수 호출
        	}
        	thr_idx++;
        	if(gThrObjs[thr_idx].cbf == NULL) thr_idx = 0;
        }
    }
}
