/*
 * cmd.h
 *
 *  Created on: Dec 2, 2025
 *      Author: marc.nicoller
 */

#ifndef CMD_H_
#define CMD_H_

typedef void (t_rspf)(char *);

void cmdHandleCommand(int rspID, char * cmd);
void cmdSendResponse(char * msg);
void cmdRegulationProcess(void);
void cmdLoop();

#endif /* CMD_H_ */
