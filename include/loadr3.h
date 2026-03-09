#ifndef MPX_LOADR3_H
#define MPX_LOADR3_H

#include <upcb.h>
#include <mpx/sys_call.h>
#include <mpx/kpcb.h>
#include <string.h>
#include <processes.h>

void load_R3(void);
    // loads the R3 test processes from <processes.h>
  

void load_R3_suspended(void);
    // loads all R3 processes but sets the status to ready suspended

void load_process(const char* name, dispatching_state dispatch);
    // loads a single process giving one of the process names proc1 -> proc5
    // the process will be loaded as a ready non-suspended process



#endif
