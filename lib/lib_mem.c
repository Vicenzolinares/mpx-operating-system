#include <lib_mem.h>

MCB_list flist = { .head = NULL, .tail = NULL };
MCB_list *free_list = &flist;
MCB_list alist = { .head = NULL, .tail = NULL };
MCB_list *allocated_list = &alist;

void *initialize_heap(size_t size)
{
    
}

void *allocate_memory(size_t size)
{
    
}

int free_memory(void *start_address)
{
    
}
