#ifndef MPX_IOCB_H
#define MPX_IOCB_H

#include <mpx/kpcb.h>


typedef enum IOCB_operation {
    READ = 0,
    WRITE = 1,
} IOCB_operation;

typedef struct IOCB {
    PCB *pcb;
    DCB *dcb;
    IOCB_operation operation;
    char* buffer;
    int buffer_size;
    struct IOCB *next;
} IOCB;

#endif
