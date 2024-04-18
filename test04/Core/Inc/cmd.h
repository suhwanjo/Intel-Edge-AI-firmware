/*
 * cmd.h
 *
 *  Created on: Apr 17, 2024
 *      Author: IOT
 */

#ifndef INC_CMD_H_
#define INC_CMD_H_

#include "main.h"

enum{
	E_CMD_LED, 	// 0
	E_CMD_CLI, 	// 1
	E_CMD_LCD,
	E_CMD_MAX 	// 2
};

#ifdef __cplusplus
extern "C" {
#endif

void cmd_init(void);
void cmd_thread(void *);

#ifdef __cplusplus
}
#endif

#endif /* INC_CMD_H_ */
