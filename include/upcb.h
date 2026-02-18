#ifndef MPX_UPCB_H
#define MPX_UPCB_H

#include <mpx/kpcb.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <sys_req.h>

int create_pcb(const char* name, PCB_class class, int priority);

/**
 Displays a person's name, class, state, suspended status, priority
 @param name The name of the pcb
 @return 0 if success, -1 if name is invalid, -2 if name does not exist
*/
int show_pcb(const char* name);

/**
 Shows all PCB information in the ready queue
*/
void show_ready(void);

/**
 Shows all PCB information in the blocked queeu
*/
void show_blocked(void);

/**
 Shows all PCB information in both queues
*/
void show_all(void);

#endif
