/*
 * button.c
 *
 *  Created on: Apr 9, 2024
 *      Author: IOT
 */
#include <stdio.h>
#include "main.h"
#include "button.h"
#include <stdbool.h>

static void button_dummy(void *); // 경고 없애기 위해


// 버튼 객체 배열
static BUTTON_T gBtnObjs[] = {
    {USER_Btn_GPIO_Port, USER_Btn_Pin, 80, 0, 0, 0, button_dummy, {true, 0}},
    {NULL , 0 , 0 , 0, 0, 0, NULL , {true, 0}}
};

// 버튼 드라이버 초기화 함수
void button_init(void)
{
    // 비어있음
}

// 버튼 이벤트 콜백 함수 등록 함수
void button_regcbf(uint16_t idx, FUNC_CBF cbf)
{
    gBtnObjs[idx].cbf = cbf;
}

// 80ms 마다 버튼 상태 체크 함수
void button_check(void)
{
    BUTTON_T *p = &gBtnObjs[0];

    // 버튼 객체 배열 순회
    for(uint8_t i=0; p->cbf != NULL; i++){
        p->count++;                  // 버튼 체크 카운터 증가(당연하게 버튼마다 count 값을 가짐)
        p->count %= p->period;       // 주기로 나눈 나머지 저장

        // 주기(period)마다 버튼 상태 체크
        if(p->count == 0){
            p->curr = HAL_GPIO_ReadPin(p->port, p->pin); // 현재 버튼 상태 읽기

            // 버튼 상태 변화 감지
            if(p->prev == 0 && p->curr == 1){ // 눌림 이벤트
                p->sts.edge = true;
                p->sts.pushed_count = 0;
                p->cbf((void *)&(p->sts)); // 콜백 함수 호출
            }
            else if(p->prev == 1 && p->curr == 0){ // 떼짐 이벤트
                p->sts.edge = false;
                p->cbf((void *)&(p->sts));
            }
            else if(p->prev == 1 && p->curr == 1){ // 누르고 있음
                if(p->sts.pushed_count < 100)
                    p->sts.pushed_count++;
            }
            else
                p->sts.pushed_count = 0;

            p->prev = p->curr; // 이전 상태 업데이트
        }

        p++; // 다음 버튼 객체로 이동
    }
}

// 빈 콜백 함수
static void button_dummy(void *)
{
    return;
}
