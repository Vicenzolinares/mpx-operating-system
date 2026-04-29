#ifndef MPX_IOCB_H
#define MPX_IOCB_H

#include <mpx/kpcb.h>
#include <mpx/serial.h>

typedef enum IOCB_operation {
    IOCB_READ = 0,
    IOCB_WRITE = 1,
} IOCB_operation;

typedef struct IOCB {
    PCB *pcb;
    struct dcb *dcb;
    IOCB_operation operation;
    char* buffer;
    int buffer_size;
    struct IOCB *next;
} IOCB;

#endif
