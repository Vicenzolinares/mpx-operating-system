#include <upcb.h>

static bool is_valid_name(const char* name)
{
    if (strlen(name) != PCB_NAME_LENGTH)
    {
        char name_err_msg[] = "Invalid name length\r\n";
        sys_req(WRITE, COM1, name_err_msg, strlen(name_err_msg));
        return false;
    }

    return true;
}

static bool is_unique_name(const char* name)
{
    if (pcb_find(name) != NULL)
    {
        char unique_err_msg[] = "PCB name must be unique\r\n";
        sys_req(WRITE, COM1, unique_err_msg, strlen(unique_err_msg));
        return false;
    }

    return true;
}

static bool is_valid_class(PCB_class class)
{
    if (class != USER && class != SYSTEM)
    {
        char class_err_msg[] = "Invalid class parameter\r\n";
        sys_req(WRITE, COM1, class_err_msg, strlen(class_err_msg));
        return false;
    }

    return true;
}

static bool is_valid_priority(int priority)
{
    if (priority < 0 || priority > 9)
    {
        char priority_err_msg[] = "Invalid priority level, must be between 0 and 9\r\n";
        sys_req(WRITE, COM1, priority_err_msg, strlen(priority_err_msg));
        return false;
    }

    return true;
}

int create_pcb(const char* name, PCB_class class, int priority)
{
    bool error = false;

    if (is_valid_name(name) == false) error = true;
    if (is_valid_class(class) == false) error = true;
    if (is_valid_priority(priority) == false) error = true;

    if (error)
    {
        char pcb_create_err_msg[] = "Invalid PCB parameters\r\n";
        sys_req(WRITE, COM1, pcb_create_err_msg, strlen(pcb_create_err_msg));
        return -1;
    }

    PCB *pcb = pcb_setup(name, class, priority);
    if (pcb == NULL)
    {
        char pcb_create_setup_err_msg[] = "Failed to set up PCB\r\n";
        sys_req(WRITE, COM1, pcb_create_setup_err_msg, strlen(pcb_create_setup_err_msg));
        return -2;
    }

    pcb_insert(pcb);
    return 0;
}
