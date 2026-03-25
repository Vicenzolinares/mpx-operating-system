#ifndef MPX_LIB_MEM_H
#define MPX_LIB_MEM_H

#include <mpx/vm.h>

typedef struct MCB {
    void *start_address;
    size_t size;
    struct MCB *next;
    struct MCB *prev;
} MCB;

typedef struct MCB_list {
    struct MCB *head;
    struct MCB *tail;
} MCB_list;

extern MCB_list *free_list;
extern MCB_list *allocated_list;

void *initialize_heap(size_t size);

void *allocate_memory(size_t size);

int free_memory(void *start_address);

#endif
