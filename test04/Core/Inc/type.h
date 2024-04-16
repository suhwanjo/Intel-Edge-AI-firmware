/*
 * type.h
 *
 *  Created on: Apr 16, 2024
 *      Author: IOT
 */

#ifndef INC_TYPE_H_
#define INC_TYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum{
	E_MSG_CLI = 0x1000,
	E_MSG_CLI_INIT,
	E_MSG_MAX
}MSG_E;

typedef struct __attribute__((packed)){ // gcc
	uint16_t id;
	union { // = type cast
		uint8_t 	b[4];
		uint16_t 	h[2];
		uint32_t 	w;
		void 		*vPtr;
		uint8_t 	*bPtr;
		uint16_t 	*hPtr;
		uint32_t 	*wPtr;
	}body;
}MSG_T;

#ifdef __cplusplus
}
#endif

#endif /* INC_TYPE_H_ */
