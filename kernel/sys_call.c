#include <mpx/sys_call.h>

PCB *current_process = NULL;
static struct context *first_context = NULL;

struct context *sys_call(struct context *cont) {
   
    io_complete();

    if (cont->eax == IDLE)
    {
         if (first_context == NULL)
        {
            first_context = cont;
        }

        PCB *new_proc = ready_queue->head;
        while (new_proc != NULL)
        {
            if (new_proc->dispatch == NOT_SUSPENDED)
            {
                pcb_remove(new_proc);
                new_proc->execution = RUNNING;

                if (current_process != NULL)
                {
                    current_process->execution = READY;
                    current_process->stack_pointer = (unsigned char *) cont;
                    pcb_insert(current_process);
                }

                current_process = new_proc;
                ((struct context *)current_process->stack_pointer)->eax = 0;
                return (struct context *)current_process->stack_pointer;
            }

            new_proc = new_proc->next;
        }

        cont->eax = 0;
        return cont;
    }
    else if (cont->eax == EXIT)
    {
        if (current_process != NULL)
        {
            pcb_free(current_process);
        }

        PCB *new_proc = ready_queue->head;
        while (new_proc != NULL)
        {
            if (new_proc->dispatch == NOT_SUSPENDED)
            {
                pcb_remove(new_proc);
                new_proc->execution = RUNNING;

                current_process = new_proc;
                ((struct context *)current_process->stack_pointer)->eax = 0;
                return (struct context *)current_process->stack_pointer;
            }

            new_proc = new_proc->next;
        }

        // fixes R3
        // current process was not cleared when there is no processes in the queue
        // was never a problem in R4, because comhand and idle were always in the queue
        current_process = NULL;

        first_context->eax = 0;
        return first_context;
    }
    // READ or Write
    else
    {
        current_process->stack_pointer = (unsigned char *) cont;
        current_process->execution = BLOCKED;
        pcb_insert(current_process);

        io_schedule(cont->ebx, cont->eax, cont->ecx, cont->edx, current_process);

        PCB *new_proc = ready_queue->head;
        while (new_proc != NULL)
        {
            if (new_proc->dispatch == NOT_SUSPENDED)
            {
                pcb_remove(new_proc);
                new_proc->execution = RUNNING;

                current_process = new_proc;
                ((struct context *)current_process->stack_pointer)->eax = 0;
                return (struct context *)current_process->stack_pointer;
            }

            new_proc = new_proc->next;
        }
    }
}
