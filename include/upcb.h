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
 @note PCB name length must be between 1 and 8 characters.
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
         -3 for system process, non-zero for failing to remove from queue
*/
int set_pcb_priority(const char* name, int priority);

/**
 Sets a PCB's execution state to blocked
 @param name The name of the PCB
 @return 0 on success, -1 for invalid name, -2 for PCB not found,
         -3 for system process, -4 if already blocked,
         non-zero from pcb_remove
*/
int block_pcb(const char* name);

/**
 Sets a PCB's execution state to ready
 @param name The name of the PCB
 @return 0 on success, -1 for invalid name, -2 for PCB not found, -3 if already ready,
         non-zero from pcb_remove
*/
int unblock_pcb(const char* name);

/**
 Sets a PCB's dispatching state to suspended
 @param name The name of the PCB
 @return 0 on success, -1 for invalid name, -2 for PCB not found,
         -3 for system process, -4 if already suspended,
         non-zero from pcb_remove
*/
int suspend_pcb(const char* name);

/**
 Sets a PCB's dispatching state to not suspended
 @param name The name of the PCB
 @return 0 on success, -1 for invalid name, -2 for PCB not found, -3 if already not suspended,
         non-zero from pcb_remove
*/
int resume_pcb(const char* name);

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

/**
 Shows all suspended PCBs across both queues
*/
void show_suspended(void);

/**
 Loads one R3 test process (proc1-proc5) as ready and non-suspended
 @param name Process name (proc1..proc5)
 @return 0 on success, non-zero on failure
*/
int loadprocess(const char *name);

/**
 Loads all R3 test processes as ready and non-suspended
 @return 0 if all processes loaded, non-zero otherwise
*/
int loadr3(void);

/**
 Loads all R3 test processes as ready and suspended
 @return 0 if all processes loaded, non-zero otherwise
*/
int loadr3suspended(void);

/**
 Resumes all ready suspended processes
 @return Number of resumed processes, or non-zero error if any resume fails
*/
int resume_all(void);

#endif
