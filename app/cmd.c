/*
 * cmd.c
 *
 *  Created on: Dec 2, 2025
 *      Author: marc.nicoller
 */
#include "usart.h"
#include "main.h"
#include "string.h"

/**

 */

#include "cmd.h"
#include "version.h"
#include "params.h"
#include "app.h"

#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#define MAX_MESSAGE_LEN 128
char cmdtext_message[MAX_MESSAGE_LEN];
uint32_t cmdtext_messageMask = 0;

bool spyEnabled=false;

static char * getWord(char * word, char sep, char * str)
{
	while(*str!=0 && *str!=sep) {
		*word++=*str++;
	}
	*word=0;

	if (*str==sep) str++;

	return str;
}

typedef void (*tCommandHandler)(int rspID, char * cmd);

typedef struct {
	const char * name; // pointer to command name
	tCommandHandler h;
	const char * helper; // pointer to help string
} tCommandEntry;

void cmdVersionHandler(int rspID, char * cmd)
{
    char rsp[80];
    sprintf(rsp,"Version %s", VERSION_STR);

    cmdSendResponse(rsp);
}

void cmdParamHandler(int rspID, char * cmd)
{
    paramExec(cmd, cmdSendResponse);
}




void cmdSpyHandler(int rspID, char * cmd)
{
    char word[40];
    char * current = getWord(word, ' ', cmd);
    if (word[0]=='1') {
      spyEnabled=true;
      cmdSendResponse("SPY enabled");
    } else {
      spyEnabled=false;
      cmdSendResponse("SPY disabled");
    }
}

const tCommandEntry commandHandlers[] = {
		{"version", cmdVersionHandler, " Return software version"},
		{"app", appCommand, "Set app command. param : 50k|5k|PWM"},
};

/* Handle a text command
 * 	Look for a keywork in the above list
 */
void cmdHandleCommand(int rspID, char * cmd)
{
	int t;
	char word[40];
	bool found = false;
	char * current = getWord(word, ' ', cmd);
	for(t=0;t<sizeof(commandHandlers)/sizeof(tCommandEntry); t++) {
		if (strcmp(word, commandHandlers[t].name)==0) {
			commandHandlers[t].h(rspID, current);
			found = true;
			break;
		}
	}
	if (!found) { // If not found, return a list of commands
		char rsp[128];
		cmdSendResponse("Available commands :");
		for(t=0;t<sizeof(commandHandlers)/sizeof(tCommandEntry); t++) {
			sprintf(rsp, "%10s %s", commandHandlers[t].name, commandHandlers[t].helper);
			cmdSendResponse(rsp);
		}

	}
}

// This is called when the process occurs, let display or not something
void cmdRegulationProcess(void)
{
  char str[80];
  if (spyEnabled) {
    sprintf(str,"undefined");
    cmdSendResponse(str);
  }
}

// Send a response over the UART
void cmdSendResponse(char * msg)
{
	sendUart(msg);
	sendUart("\n");
}

void cmdLoop()
{
	char str[128];

	if (getUart(str)>0) {
		cmdHandleCommand(0,str);
	}
}
