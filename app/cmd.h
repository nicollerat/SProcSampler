/*
 * cmd.h
 *
 *  Created on: Dec 2, 2025
 *      Author: marc.nicoller
 */

#ifndef CMD_H_
#define CMD_H_

typedef void (*t_rspf)(char *);

void cmdHandleCommand(char * cmd, t_rspf rspf);
void cmdSendResponse(char * msg, t_rspf rspf);
void cmdRegulationProcess(void);
void cmdLoop();

#endif /* CMD_H_ */
