#include <mpx/kpcb.h>

PCB_queue ready_queue = { .head = NULL, .tail = NULL };
PCB_queue blocked_queue = { .head = NULL, .tail = NULL };

PCB* pcb_allocate(void)
{
    PCB *pcb = sys_alloc_mem(sizeof(PCB));
    if (pcb == NULL)
    {
        return NULL;
    }

    pcb->stack = sys_alloc_mem(PCB_STACK_SIZE);
    if (pcb->stack == NULL)
    {
        sys_free_mem(pcb);
        return NULL;
    }

    memset(pcb->stack, 0, PCB_STACK_SIZE);
    
    return pcb;
}

int pcb_free(PCB *pcb)
{
    int result = sys_free_mem(pcb->stack);
    if (result != 0)
    {
        return result;
    }

    return sys_free_mem(pcb);
}

PCB* pcb_setup(const char *name, PCB_class class, int priority)
{
    if (strlen(name) != PCB_NAME_LENGTH)
    {
        return NULL;
    }

    if (class != USER && class != SYSTEM)
    {
        return NULL;
    }

    if(priority < 0 || priority > 9)
    {
        return NULL;
    }

    PCB *pcb = pcb_allocate();
    if (pcb == NULL)
    {
        return NULL;
    }

    memcpy(pcb->name, name, PCB_NAME_LENGTH);
    pcb->name[PCB_NAME_LENGTH] = '\0';
    pcb->class = class;
    pcb->priority = priority;
    pcb->execution = READY;
    pcb->dispatch = NOT_SUSPENDED;
    pcb->stack_pointer = pcb->stack + PCB_STACK_SIZE - sizeof(void *);
    pcb->next = NULL;
    pcb->prev = NULL;

    return pcb;
}

PCB* pcb_find(const char *name)
{
    PCB *current = ready_queue.head;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            return current;
        }
        current = current->next;
    }

    current = blocked_queue.head;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            return current;
        }
        current = current->next;
    }

    return NULL;
}
