/*
 * cmd.c
 *
 *  Created on: Dec 2, 2025
 *      Author: marc.nicoller
 */
#include "usart.h"
#include "main.h"
#include "string.h"
#include "stddef.h"
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

typedef void (*tCommandHandler)(char * cmd, t_rspf rspf);

typedef struct {
	const char * name; // pointer to command name
	tCommandHandler h;
	const char * helper; // pointer to help string
} tCommandEntry;

void cmdVersionHandler(char * cmd, t_rspf rspf)
{
    char rsp[80];
    sprintf(rsp,"Version %s", VERSION_STR);

    cmdSendResponse(rsp, rspf);
}

void cmdParamHandler(int rspID, char * cmd, t_rspf rspf)
{
    paramExec(cmd, rspf);
}


const tCommandEntry commandHandlers[] = {
		{"version", cmdVersionHandler, " Return software version"},
		{"app", appCommand, "Set app command. param : 50k|5k|PWM"},
		{"p", paramExec, "Handle parameters"},
};

/* Handle a text command
 * 	Look for a keywork in the above list
 */
void cmdHandleCommand(char * cmd, t_rspf rspf)
{
	int t;
	char word[40];
	bool found = false;
	char * current = getWord(word, ' ', cmd);
	for(t=0;t<sizeof(commandHandlers)/sizeof(tCommandEntry); t++) {
		if (strcmp(word, commandHandlers[t].name)==0) {
			commandHandlers[t].h(current, rspf);
			found = true;
			break;
		}
	}
	if (!found) { // If not found, return a list of commands
		char rsp[128];
		cmdSendResponse("Available commands :", rspf);
		for(t=0;t<sizeof(commandHandlers)/sizeof(tCommandEntry); t++) {
			sprintf(rsp, "%10s %s", commandHandlers[t].name, commandHandlers[t].helper);
			cmdSendResponse(rsp, rspf);
		}

	}
}

t_rspf proc_rspf = 0;

// This is called when the process occurs, let display or not something
void cmdRegulationProcess(void)
{
  char str[80];
  if (spyEnabled) {
    sprintf(str,"undefined");
    if (proc_rspf) {
    	cmdSendResponse(str, proc_rspf);
    }
  }
}

// Send a response over the UART
void cmdSendResponse(char * msg, t_rspf rspf)
{
	rspf(msg);
	rspf("\n");
}

// Background loop that handle messages from the interfaces
//   here we consider the UART interface linked to the debugger connection
void cmdLoop()
{
	char str[128];

	if (getUart(str)>0) {
		cmdHandleCommand(str, sendUart);
	}
}
