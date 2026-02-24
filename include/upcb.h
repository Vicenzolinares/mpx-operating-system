#ifndef MPX_UPCB_H
#define MPX_UPCB_H

#include <mpx/kpcb.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <sys_req.h>

/**
 @file upcb.h
 @brief User-level PCB functions
*/

/**
 Creates a new PCB and adds it to the proper queue
 @param name The name of the PCB
 @param class The class of the PCB (USER or SYSTEM)
 @param priority The priority level of the PCB (0-9)
 @return 0 on success, -1 for invalid parameters, -2 for failed PCB creation
*/
int create_pcb(const char* name, PCB_class class, int priority);

/**
 Removes a PCB from its queue and frees its memory, cannot be a system process
 @param name The name of the PCB to delete
 @return 0 on success, -1 for invalid name, -2 for PCB not found,
         -3 for system process, non-zero from pcb_remove or pcb_free
*/
int delete_pcb(const char* name);

/**
 Sets the priority of a PCB
 @param name The name of the PCB
 @param priority The new priority level (0-9)
 @return 0 on success, -1 for invalid parameters, -2 for PCB not found,
         non-zero for failing to remove from queue
*/
int set_pcb_priority(const char* name, int priority);

/**
 Blocks a READY PCB, changing its state to BLOCKED
 @param name The name of the PCB to block
 @return 0 on success, -1 for invalid name, -2 for PCB not found, -3 for invalid state
*/
int block_pcb(const char* name);

/**
 Unblocks a BLOCKED PCB, changing its state to READY
 @param name The name of the PCB to unblock
 @return 0 on success, -1 for invalid name, -2 for PCB not found, -3 for invalid state
*/
int unblock_pcb(const char* name);

/**
 Suspends a NOT_SUSPENDED PCB, changing its dispatch state to SUSPENDED
 @param name The name of the PCB to suspend
 @return 0 on success, -1 for invalid name, -2 for PCB not found, -3 for invalid state
*/
int suspend_pcb(const char* name);

/**
 Resumes a SUSPENDED PCB, changing its dispatch state to NOT_SUSPENDED
 @param name The name of the PCB to resume
 @return 0 on success, -1 for invalid name, -2 for PCB not found, -3 for invalid state
*/
int resume_pcb(const char* name);

/** 
 Displays a person's name, class, state, suspended status, priority
 @param name The name of the pcb
 @return 0 if success, -1 if name is invalid, -2 if name does not exist
*/
int show_pcb(const char* name);

/**formation in the ready queue
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
