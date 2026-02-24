/*
 * button.c
 *
 *  Created on: Feb 19, 2026
 *      Author: marc.nicoller
 */
#include "button.h"
#include "gpio.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>

uint32_t stamp=0;

bool btnLoop()
{
	bool ret=false;

	bool state = HAL_GPIO_ReadPin(S1_GPIO_Port, S1_Pin);
	if (state) {
		if (stamp==0) ret=true;
		stamp=HAL_GetTick();
	} else {
		if (stamp>0) {
			if (HAL_GetTick()-stamp>10) stamp=0;
		}
	}
	return ret;
}
