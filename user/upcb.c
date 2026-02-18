#include <upcb.h>

/**
 Checks if a PCB name is valid, error message if false
 @param name name of the PCB to check
 @return true if the name is valid, false otherwise
*/
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

/**
 Checks if a PCB name is unique, error message if false
 @param name name of the PCB to check
 @return true if the name is unique, false otherwise
*/
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

/**
 Checks if a PCB class is valid, error message if false
 @param class class of the PCB to check
 @return true if the class is valid, false otherwise
*/
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

/**
 Checks if a PCB priority is valid, error message if false
 @param priority priority of the PCB to check
 @return true if the priority is valid, false otherwise
*/
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

/**
 Checks if a PCB is a system process
 @param pcb the PCB to check
 @return true if the PCB is a system process, false otherwise
*/
static bool is_system_process(PCB *pcb)
{
    return pcb->class == SYSTEM;
}

int create_pcb(const char* name, PCB_class class, int priority)
{
    // parrameter checking
    bool error = false;

    if (is_valid_name(name) == false)
    {
        error = true;
    } else if (is_unique_name(name) == false)
    {
        error = true;
    }
    if (is_valid_class(class) == false) error = true;
    if (is_valid_priority(priority) == false) error = true;

    if (error)
    {
        char pcb_create_param_err_msg[] = "Invalid PCB parameters\r\n";
        sys_req(WRITE, COM1, pcb_create_param_err_msg, strlen(pcb_create_param_err_msg));
        return -1;
    }

    // PCB creation
    PCB *pcb = pcb_setup(name, class, priority);
    if (pcb == NULL)
    {
        char pcb_create_setup_err_msg[] = "Failed to create PCB\r\n";
        sys_req(WRITE, COM1, pcb_create_setup_err_msg, strlen(pcb_create_setup_err_msg));
        return -2;
    }

    pcb_insert(pcb);
    return 0;
}

int delete_pcb(const char* name)
{
    // check for valid name
    if (is_valid_name(name) == false)
    {
        return -1;
    }

    // check for PCB existence
    PCB *pcb = pcb_find(name);
    if (pcb == NULL)
    {
        char pcb_not_found_err_msg[] = "PCB not found\r\n";
        sys_req(WRITE, COM1, pcb_not_found_err_msg, strlen(pcb_not_found_err_msg));
        return -2;
    }

    // check for system process
    if (is_system_process(pcb))
    {
        char cannot_delete_sys_err_msg[] = "Cannot delete system process\r\n";
        sys_req(WRITE, COM1, cannot_delete_sys_err_msg, strlen(cannot_delete_sys_err_msg));
        return -3;
    }

    // remove from queue
    int result = pcb_remove(pcb);
    if (result != 0)
    {
        char pcb_remove_err_msg[] = "Failed to remove PCB from queue\r\n";
        sys_req(WRITE, COM1, pcb_remove_err_msg, strlen(pcb_remove_err_msg));
        return result;
    }

    // free memory
    result = pcb_free(pcb);
    if (result != 0)
    {
        char pcb_free_err_msg[] = "Failed to free PCB memory\r\n";
        sys_req(WRITE, COM1, pcb_free_err_msg, strlen(pcb_free_err_msg));
        return result;
    }

    return 0;
}

int set_pcb_priority(const char* name, int priority)
{
    // parameter checking
    bool error = false;

    if(is_valid_name(name) == false) error = true;
    if(is_valid_priority(priority) == false) error = true;

    if (error)
    {
        char pcb_set_priority_param_err_msg[] = "Invalid parameters for setting PCB priority\r\n";
        sys_req(WRITE, COM1, pcb_set_priority_param_err_msg, strlen(pcb_set_priority_param_err_msg));
        return -1;
    }

    // get PCB
    PCB *pcb = pcb_find(name);
    if (pcb == NULL)
    {
        char pcb_not_found_err_msg[] = "PCB not found\r\n";
        sys_req(WRITE, COM1, pcb_not_found_err_msg, strlen(pcb_not_found_err_msg));
        return -2;
    }

    // remove from queue
    int remove = pcb_remove(pcb);
    if (remove != 0)
    {
        char pcb_remove_err_msg[] = "Failed to remove PCB from queue\r\n";
        sys_req(WRITE, COM1, pcb_remove_err_msg, strlen(pcb_remove_err_msg));
        return remove;
    }

    pcb->priority = priority;
    pcb_insert(pcb);
    return 0;
}
