/*
 * button.h
 *
 *  Created on: Apr 9, 2024
 *      Author: IOT
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "main.h"
#include <stdbool.h>

// 버튼 상태를 나타내는 구조체
typedef struct{
    bool edge;           // 버튼 상태 변화 여부
    uint16_t pushed_count; // 버튼 눌림 횟수
} BUTTON_STS;

// 버튼 이벤트 콜백 함수 타입
typedef void(*FUNC_CBF)(void*);

// 버튼 객체 구조체
typedef struct{
    GPIO_TypeDef *port;  // 버튼 GPIO 포트
    uint16_t pin;        // 버튼 GPIO 핀
    uint32_t period;     // 버튼 체크 주기
    uint32_t count;      // 버튼 체크 카운터
    uint8_t prev;        // 이전 버튼 상태
    uint8_t curr;        // 현재 버튼 상태
    FUNC_CBF cbf;        // 버튼 이벤트 콜백 함수
    BUTTON_STS sts;      // 버튼 상태
} BUTTON_T;

#ifdef __cplusplus
extern "C"{
#endif

void button_init(void);
void button_regcbf(uint16_t idx, FUNC_CBF cbf);
void button_check(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_APP_H_ */
