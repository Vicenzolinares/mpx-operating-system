#include <mpx/sys_call.h>

PCB *current_process = NULL;
static struct context *first_context = NULL;

struct context *sys_call(struct context *cont) {
    if (first_context == NULL)
    {
        first_context = cont;
    }

    if (cont->eax == IDLE)
    {
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

        ((struct context *)current_process->stack_pointer)->eax = 0;
        return (struct context *)current_process->stack_pointer;
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

        first_context->eax = 0;
        return first_context;
    }
    else
    {
        cont->eax = -1;
        return cont;
    }
}
