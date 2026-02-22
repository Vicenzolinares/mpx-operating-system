#include <mpx/kpcb.h>

PCB_queue rqueue = { .head = NULL, .tail = NULL };
PCB_queue *ready_queue = &rqueue;
PCB_queue bqueue = { .head = NULL, .tail = NULL };
PCB_queue *blocked_queue = &bqueue;

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
    PCB *current = ready_queue->head;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            return current;
        }
        current = current->next;
    }

    current = blocked_queue->head;
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

static void queue_insert(PCB *pcb, PCB_queue *queue)
{
    //helper 

    //empty queue
    if (queue->head == NULL)
    {   
        queue->head = pcb;
        queue->tail = pcb;
        return;
    }

    //insert at front node (not empty queue)
    if(pcb->priority < queue->head->priority)
    {
        pcb->next = queue->head;
        queue->head->prev = pcb;
        queue->head = pcb;
        return;
    }
    
    //iteration sequence
    PCB* current = queue->head;

    while(current->next != NULL && current->next->priority <= pcb->priority)
    {
        //iterate
        current = current->next;
    }

    //insert at tail
    if(current->next == NULL)
    {
        queue->tail->next = pcb;
        pcb->prev = queue->tail;
        queue->tail = pcb;
        return;
    }

    //insert inbetween
    if(current->next->priority > pcb->priority)
    {
        pcb->prev = current;
        pcb->next = current->next;
        current->next = pcb;
        pcb->next->prev = pcb;
        return; 
    } 
}

void pcb_insert(PCB *pcb)
{
    if(pcb->execution == READY)
    {
        queue_insert(pcb, ready_queue);
    }
    if(pcb->execution == BLOCKED)
    {
        queue_insert(pcb, blocked_queue);    
    }
}

static void queue_remove(PCB *pcb, PCB_queue *queue)
{
    // remove head and tail (only one node in queue)
    if (pcb == queue->tail && pcb == queue->head)
    {
        queue->head = NULL;
        queue->tail = NULL;
        return;
    }

    //remove head node (not empty queue)
    if(pcb == queue->head)
    {
        queue->head = pcb->next;
        queue->head->prev = NULL;
        pcb->next = NULL;
        return;
    }
    
    //iteration sequence
    PCB* current = queue->head;

    while(current->next != NULL && current->next != pcb)
    {
        //iterate
        current = current->next;
    }

    //remove tail node
    if(pcb->next == NULL)
    {
        queue->tail = pcb->prev;
        queue->tail->next = NULL;
        pcb->prev = NULL;
        return;
    }

    //remove inbetween node
    if(current->next == pcb)
    {
        current->next = pcb->next;
        pcb->next->prev = current;
        pcb->next = NULL;
        pcb->prev = NULL;
        return; 
    }
}

int pcb_remove(PCB *pcb)
{
    if(pcb == NULL)
    {
        return -1;
    }
    if(pcb->execution == RUNNING)
    {
        return -2;
    }
    if(pcb->execution == READY)
    {
        queue_remove(pcb, ready_queue);
    }
    if(pcb->execution == BLOCKED)
    {
        queue_remove(pcb, blocked_queue);   
    }
    return 0;
}
