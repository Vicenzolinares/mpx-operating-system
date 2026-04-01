#include <lib_mem.h>

MCB_list flist = { .head = NULL, .tail = NULL };
MCB_list *free_list = &flist;
MCB_list alist = { .head = NULL, .tail = NULL };
MCB_list *allocated_list = &alist;

void *initialize_heap(size_t size)
{
    void* heap = kmalloc(size, 0, NULL);

    MCB* initial_block = (MCB*) heap;
    initial_block->start_address = (void*)((int)heap + sizeof(MCB));
    initial_block->size = size - sizeof(MCB);
    initial_block->next = NULL;
    initial_block->prev = NULL;

    free_list->head = initial_block;
    free_list->tail = initial_block;

    return heap;
}

void *allocate_memory(size_t size)
{
    
}

int free_memory(void *start_address)
{
    
}
