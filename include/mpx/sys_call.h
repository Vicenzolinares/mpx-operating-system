#ifndef MPX_SYS_CALL_H
#define MPX_SYS_CALL_H

#include <mpx/kpcb.h>
#include <sys_req.h>
#include <string.h>
#include <stdint.h>

/**
 @file sys_call.h
 @brief System call interface definitions
*/

struct context {
    // last regester to get pushed
    int32_t esp;

    // segment registers
    int16_t ds;
    int16_t es;
    int16_t fs;
    int16_t gs;
    int16_t ss;

    // general purpose registers
    int32_t eax;
    int32_t ebx;
    int32_t ecx;
    int32_t edx;
    int32_t esi;
    int32_t edi;
    int32_t ebp;

    // cpu pushes these automatically
    int32_t eflags;
    int16_t cs;
    int32_t eip;
};

struct context *sys_call(struct context *cont);

#endif
