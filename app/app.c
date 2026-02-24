/*
 * app.c
 *
 *  Created on: Nov 2, 2025
 *      Author: marc.nicoller
 */
#include "app.h"
#include "dac.h"
#include "main.h"
#include "button.h"
#include "siggen.h"
#include "cmd.h"

#include "tim.h"
#include "gpio.h"
#include <string.h>

int cnt=0;

typedef enum  {APP_COPY, APP_PWM, APP_SIGGEN} e_app ;

e_app APP=APP_COPY;


e_mode MODE=MODE_50k;

// PWM app will output as a PWM signal the value
void pwmUpdate(uint16_t value)
{
	uint32_t reg = (value*htim1.Instance->ARR)>>12;
	htim1.Instance->CCR1=reg;
}

#define TAB_SIZE 1024
uint16_t tabValues1[TAB_SIZE];
uint16_t tabValues2[TAB_SIZE];
int tabIndex = 0;
int cntl =0;
// DUMP the values
//   dump binary value value_3V_5.bin tabValues1
// read in python with
//   values = np.fromfile('value1.bin', dtype=np.int16)
int prev_v=0xfff;
int subdiv=0;

// Callback from the ADC
void app_adc(uint16_t value1, uint16_t value2)
{
	HAL_GPIO_WritePin(RE_LED2_GPIO_Port, RE_LED2_Pin, 1);
	cnt++;
	tabValues1[tabIndex]=value1;
	tabValues2[tabIndex]=value2;

	// trigger around mid value
	if (tabIndex==0) {
		if (value1>=0x800 && prev_v<0x800) {
			tabIndex=1;
		}
	} else {
		tabIndex = (tabIndex+1)%TAB_SIZE;
		if (tabIndex==0) {
			cntl++;
		}
	}
	prev_v=value1;

	switch(APP) {
	case APP_COPY:
		if (MODE==MODE_5k) {
			subdiv++;
			if (subdiv>=10) {
				subdiv=0;
			} else {
				break;
			}
		}
		writeDAC1(value1);
		writeDAC2(value2);
		if (MODE==MODE_PWM) {
			pwmUpdate(value1);
		} else {
			pwmUpdate(0);
		}
		break;

	case APP_PWM:
		pwmUpdate(value1);
		break;

	case APP_SIGGEN:
		sgUpdate();
	}

	HAL_GPIO_WritePin(RE_LED2_GPIO_Port, RE_LED2_Pin, 0);
}

void app_init()
{
	APP = APP_COPY;
	setPWMFreq(50000);
	enablePWM(true);
}

void app_loop()
{
	cmdLoop();

	if (btnLoop()) {
		switch(MODE) {
		case MODE_50k:
			app_setMode(MODE_5k);
			break;
		case MODE_5k:
			app_setMode(MODE_PWM);
			break;
		case MODE_PWM:
			app_setMode(MODE_50k);
			break;
		}

	}
}

// Handle commands from UART
void appCommand(int rspID, char * str)
{
	if (strcmp(str,"pwm")==0) {
		APP=APP_PWM;
		sendUart("PWM MODE\n");
	} else if (strcmp(str,"dac")==0) {
		APP=APP_COPY;
		pwmUpdate(0);
		sendUart("DAC MODE\n");
	} else if (strcmp(str,"merci qui ?")==0) {
		APP=APP_COPY;
		pwmUpdate(0);
		sendUart("Merci Pascal\n");
	} else if (strcmp(str,"help")==0) {
		APP=APP_COPY;
		pwmUpdate(0);
		sendUart("Lis la doc !\n");
	} else if (strcmp(str,"50k")==0) {
		app_setMode(MODE_50k);
	} else if (strcmp(str,"5k")==0) {
		app_setMode(MODE_5k);
	} else if (strcmp(str,"PWM")==0) {
		app_setMode(MODE_PWM);
	} else {
		sendUart("Unknown command ");
		sendUart(str);
		sendUart("\n");
	}
}

void app_setMode(e_mode mode)
{
	MODE=mode;
	switch(MODE) {
		case MODE_50k:
			HAL_GPIO_WritePin(RE_LED1_GPIO_Port, RE_LED1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GR_LED4_GPIO_Port, GR_LED4_Pin, GPIO_PIN_RESET);
			sendUart("MODE 50k\n");
			break;
		case MODE_5k:
			HAL_GPIO_WritePin(RE_LED1_GPIO_Port, RE_LED1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GR_LED4_GPIO_Port, GR_LED4_Pin, GPIO_PIN_SET);
			sendUart("MODE 5k\n");
			break;
		case MODE_PWM:
			HAL_GPIO_WritePin(RE_LED1_GPIO_Port, RE_LED1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GR_LED4_GPIO_Port, GR_LED4_Pin, GPIO_PIN_RESET);
			sendUart("MODE PWM\n");
			break;
	}
}

void app_timer(TIM_HandleTypeDef * htim)
{

}


