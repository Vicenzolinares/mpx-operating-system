#include <loadr3.h>

void load_R3(void)
{
    // loads the R3 test processes from <processes.h>
    load_process("proc1", NOT_SUSPENDED);
    load_process("proc2", NOT_SUSPENDED);
    load_process("proc3", NOT_SUSPENDED);
    load_process("proc4", NOT_SUSPENDED);
    load_process("proc5", NOT_SUSPENDED);
}
    
void load_R3_suspended(void)
{
    // loads all R3 processes but sets the status to ready suspended
    load_process("proc1", SUSPENDED);
    load_process("proc2", SUSPENDED);
    load_process("proc3", SUSPENDED);
    load_process("proc4", SUSPENDED);
    load_process("proc5", SUSPENDED);
}

void load_process(const char* name, dispatching_state dispatch)
{
    //error message check
    if (strcmp(name, "proc1") != 0 
        && strcmp(name, "proc2") != 0
        && strcmp(name, "proc3") != 0
        && strcmp(name, "proc4") != 0
        && strcmp(name, "proc5") != 0)
    {
        char strcmp_err_msg[] = "Name invalid";
        sys_req(WRITE, COM1, strcmp_err_msg, strlen(strcmp_err_msg));
        return;
    }
    // loads a single process giving one of the process names proc1 -> proc5
    // the process will be loaded as a ready non-suspended process
    create_pcb(name, USER, 5);
    if (dispatch == SUSPENDED)
    {
        suspend_pcb(name);
    }
    PCB* pcb = pcb_find(name);
    pcb->stack_pointer = pcb->stack_pointer - sizeof(struct context);
    struct context* context = (struct context*) pcb->stack_pointer;
    context->cs = 0x08;
    context->ds = 0x10;
    context->es = 0x10;
    context->fs = 0x10;
    context->gs = 0x10;
    context->ss = 0x10;
    context->ebp = pcb->stack[PCB_STACK_SIZE - 1];
    if (strcmp(name, "proc1") == 0)
    {
        context->eip = (intptr_t)proc1;
    }
    else if (strcmp(name, "proc2") == 0)
    {
        context->eip = (intptr_t)proc2;
    }
    else if (strcmp(name, "proc3") == 0)
    {
        context->eip = (intptr_t)proc3;
    }
    else if (strcmp(name, "proc4") == 0)
    {
        context->eip = (intptr_t)proc4;
    }
    else
    {
        context->eip = (intptr_t)proc5;
    }
    context->eflags = 0x0202;
    context->eax = 0;
    context->ebx = 0;
    context->ecx = 0;
    context->edx = 0;
    context->esi = 0;
    context->edi = 0;
}   

void resume_all(void)
{
    // resumes all ready suspended processes
    PCB* pcb = ready_queue->head;
    while (pcb != NULL)
    {
        if (pcb->dispatch == SUSPENDED)
        {
            pcb->dispatch = NOT_SUSPENDED;
        }
        pcb = pcb->next;
     }
}
