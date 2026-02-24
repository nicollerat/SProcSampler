/*
 * params.h
 *
 *  Created on: 1 nov. 2018
 *      Author: Marc
 */

#ifndef PARAMS_H_
#define PARAMS_H_

#include "cmd.h"

typedef enum { ptFloat, ptInt32, ptInt16} pType;

void paramAdd(char * name, pType t, void*p);
void paramExec(char * cmd, t_rspf rspf);

#endif /* PARAMS_H_ */
