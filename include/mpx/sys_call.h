#ifndef MPX_SYS_CALL_H
#define MPX_SYS_CALL_H

#include <mpx/kpcb.h>
#include <sys_req.h>
#include <string.h>
#include <stdint.h>
#include <mpx/io_scheduler.h>

/**
 @file sys_call.h
 @brief System call interface definitions
*/
struct context {
    // segment registers
    int ds;
    int es;
    int fs;
    int gs;
    int ss;

    // general purpose registers
    int eax;
    int ebx;
    int ecx;
    int edx;
    int esi;
    int edi;
    int ebp;

    // cpu pushes these automatically
    int eip;
    int cs; 
    int eflags;
};

/**
 Swaps the current process during an isr
 @param cont The context of the process that is being swapped out
 @return The context of the process that is being swapped in
*/
struct context *sys_call(struct context *cont);

#endif
