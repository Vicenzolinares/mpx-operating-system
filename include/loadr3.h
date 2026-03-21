#ifndef MPX_LOADR3_H
#define MPX_LOADR3_H

#include <upcb.h>
#include <mpx/sys_call.h>
#include <mpx/kpcb.h>
#include <string.h>
#include <processes.h>

/**
 Loads all R3 processes from <processes.h>
*/
void load_R3(void);
  
/**
 Loads all R3 processes and sets their statuses to READY SUSPENDED
*/
void load_R3_suspended(void);

/**
 Loads a single process, gives them one of the names proc1 -> proc5, loads them as a READY NON-SUSPENDED process
 @param name The name of the R3 process
 @param dispatch The dispatching state of the R3 process
*/  
void load_process(const char* name, dispatching_state dispatch);

/**
 Resumes all suspended processes in the ready queue
*/
void resume_all(void);
  
#endif
