/*
 * app.h
 *
 *  Created on: Nov 2, 2025
 *      Author: marc.nicoller
 */

#ifndef APP_APP_H_
#define APP_APP_H_

#include <stdint.h>

typedef enum { MODE_50k, MODE_5k, MODE_PWM } e_mode;
void app_setMode(e_mode mode);

void app_adc(uint16_t value1, uint16_t value2);

void app_init();
void app_loop();
void app_timer();

void appCommand(int rspID, char * str);

#endif /* APP_APP_H_ */
