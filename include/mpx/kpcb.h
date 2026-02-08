#ifndef MPX_KPCB_H
#define MPX_KPCB_H

#include <stddef.h>

typedef enum {
    USER = 0,
    SYSTEM = 1,
} PCB_class;

typedef enum {
    READY = 0,
    RUNNING = 1,
    BLOCKED = 2,
} execution_state;

typedef enum {
    NOT_SUSPENDED = 0,
    SUSPENDED = 1,
} dispatching_state;

typedef struct {
    char name[9];
    PCB_class class;
    int priority;
    execution_state execution;
    dispatching_state dispatch;
    struct PCB *next;
    struct PCB *prev;
} PCB;

typedef struct {
    struct PCB *head;
    struct PCB *tail;
} PCB_queue;

extern PCB_queue ready_queue;
extern PCB_queue blocked_queue;

#endif
