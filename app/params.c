/*
 * params.c
 *
 *  Created on: 1 nov. 2018
 *      Author: Marc
 */

#include "params.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef union {
	float f;
	int16_t i16;
	int32_t i32;
	uint32_t u32;
} tParamPointerType;

typedef struct {
	char * name;
	pType type;
	tParamPointerType * p;
}tParamEntry;

#define MAX_PARAM 32

tParamEntry paramList [MAX_PARAM];
int nbParams=0;

/** @brief Get the first word of str
 *
 * 	Copy the work into word
 * 	return the pointer of the next char of the string
 */
char * paramGetWord(char *str, char sep, char * word)
{
	while (*str!=sep && *str!=0) *word++=*str++;
	if (*str!=0) str++; // skip separator
	*word=0; // terminate string
	return str;
}

/** @brief Add a parameter to the parameter list
 *
 */
void paramAdd(char * name, pType t, void*p)
{
	if (nbParams<MAX_PARAM) {
		paramList[nbParams].name = name;
		paramList[nbParams].type = t;
		paramList[nbParams].p = p;
		nbParams++;
	}
}

/**  @brief set the value of a parameter
 *
 * 	cmd has the format "name value"
 */
void paramSet(char * cmd,t_rspf rspf)
{
	char name[20];
	int t;
	int n=0;

	cmd=paramGetWord(cmd, ' ', name); // get the name
        // rspf(name);
	for(t=0;t<nbParams;t++) {
		if (strcmp(name, paramList[t].name)==0) {
			switch(paramList[t].type) {
			case ptFloat:
				n=sscanf(cmd, "%f", (float*)&paramList[t].p->f);
				break;
			case ptInt32:
				n=sscanf(cmd, "%d", (int*)&paramList[t].p->i32);
				break;
			case ptInt16:
				n=sscanf(cmd, "%hd", &paramList[t].p->i16);
				break;
			}

			// setup a response
			if (n==1)  {
				rspf("OK\n");
			} else {
				rspf("Param : Bad value\n");
                                rspf(cmd);
			}
			return; // exit the loop
		}
	}

	rspf("Param : Not found");

}

/** @brief Add to str the value of the parameter
 *
 */
void paramGetValue(tParamEntry * pe, char * rsp)
{
       
	switch(pe->type) {
	case ptFloat:
		sprintf(rsp, "%f", (pe->p->f));
		break;
	case ptInt32:
		sprintf(rsp, "%d", (int)(pe->p->i32));
		break;
	case ptInt16:
		sprintf(rsp, "%hd", (pe->p->i16));
		break;
	default:
		sprintf(rsp, "%x", (unsigned int)(pe->p->i32));
		break;
	}
}

/** @brief get the value of a parameter
 *
 * 	cmd has the format "name"
 */
void paramGet(char * cmd, t_rspf rspf)
{
	char name[20];
	char val[20];
        char rsp[80];
	int t;

	cmd=paramGetWord(cmd, ' ', name); // get the name

	for(t=0;t<nbParams;t++) {
		if (strcmp(name, paramList[t].name)==0) {
			paramGetValue(&paramList[t], val);
                        sprintf(rsp, "%s = %s\n", paramList[t].name, val);
                        rspf(rsp);
			return; // Exit the loop
		}
	}

	rspf("Param : Not found\n");

}

void paramDump(t_rspf rspf)
{
	int t;
	char val[20];
        char rsp[80];

	for(t=0;t<nbParams;t++) {
		paramGetValue(&paramList[t], val);
		sprintf(rsp, "%s = %s\n", paramList[t].name, val);
                rspf(rsp);
	}
}

/** @brief Execute a command to change the value of the parameters
 *
 *	cmd is a string : set name value
 *		get name
 *		list
 */
void paramExec(char * cmd, t_rspf rspf)
{
	char word[20];
	cmd=paramGetWord(cmd, ' ', word); // get command
        // rspf(word);
	if (strcmp(word,"set")==0) {
          paramSet(cmd, rspf);
	} else if (strcmp(word, "get")==0) {
          paramGet(cmd, rspf);
	} else if (strcmp(word, "list")==0) {
          paramDump(rspf);
	} else {
          rspf("Param : unknown command (set|get|list)\n");
	}
}

