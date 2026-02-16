#ifndef MPX_UPCB_H
#define MPX_UPCB_H

#include <mpx/kpcb.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <sys_req.h>

int create_pcb(const char* name, PCB_class class, int priority);

#endif
