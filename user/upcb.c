#include <upcb.h>

/**
 Checks if a PCB name is valid, error message if false
 @param name name of the PCB to check
 @return true if the name is valid, false otherwise
*/
static bool is_valid_name(const char* name)
{
    if (strlen(name) > PCB_NAME_LENGTH || strlen(name) <= 0)
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

    if(is_system_process(pcb))
    {
        char cannot_change_sys_err_msg[] = "Cannot change priority of system process\r\n";
        sys_req(WRITE, COM1, cannot_change_sys_err_msg, strlen(cannot_change_sys_err_msg));
        return -3;
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

int block_pcb(const char* name)
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

    if(is_system_process(pcb))
    {
        char cannot_block_sys_err_msg[] = "Cannot block system process\r\n";
        sys_req(WRITE, COM1, cannot_block_sys_err_msg, strlen(cannot_block_sys_err_msg));
        return -3;
    }

    // must be READY to block
    if (pcb->execution != READY)
    {
        char invalid_state_err_msg[] = "PCB needs to be in READY state to block\r\n";
        sys_req(WRITE, COM1, invalid_state_err_msg, strlen(invalid_state_err_msg));
        return -4;
    }

    // remove from ready queue
    int result = pcb_remove(pcb);
    if (result != 0)
    {
        char pcb_remove_err_msg[] = "Failed to remove PCB from queue\r\n";
        sys_req(WRITE, COM1, pcb_remove_err_msg, strlen(pcb_remove_err_msg));
        return result;
    }

    // change to BLOCKED and insert into blocked queue
    pcb->execution = BLOCKED;
    pcb_insert(pcb);
    return 0;
}

int unblock_pcb(const char* name)
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

    // must be BLOCKED to unblock
    if (pcb->execution != BLOCKED)
    {
        char invalid_state_err_msg[] = "PCB must be in BLOCKED state to unblock\r\n";
        sys_req(WRITE, COM1, invalid_state_err_msg, strlen(invalid_state_err_msg));
        return -3;
    }

    // remove from blocked queue
    int result = pcb_remove(pcb);
    if (result != 0)
    {
        char pcb_remove_err_msg[] = "Failed to remove PCB from queue\r\n";
        sys_req(WRITE, COM1, pcb_remove_err_msg, strlen(pcb_remove_err_msg));
        return result;
    }

    // change to READY and insert into ready queue
    pcb->execution = READY;
    pcb_insert(pcb);
    return 0;
}

int suspend_pcb(const char* name)
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

    if(is_system_process(pcb))
    {
        char cannot_suspend_sys_err_msg[] = "Cannot suspend system process\r\n";
        sys_req(WRITE, COM1, cannot_suspend_sys_err_msg, strlen(cannot_suspend_sys_err_msg));
        return -3;
    }

    // must be NOT_SUSPENDED to suspend
    if (pcb->dispatch != NOT_SUSPENDED)
    {
        char invalid_state_err_msg[] = "PCB must be in NOT_SUSPENDED state to suspend\r\n";
        sys_req(WRITE, COM1, invalid_state_err_msg, strlen(invalid_state_err_msg));
        return -4;
    }

    // change to SUSPENDED
    pcb->dispatch = SUSPENDED;
    return 0;
}

int resume_pcb(const char* name)
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

    // must be SUSPENDED to resume
    if (pcb->dispatch != SUSPENDED)
    {
        char invalid_state_err_msg[] = "PCB must be in SUSPENDED state to resume\r\n";
        sys_req(WRITE, COM1, invalid_state_err_msg, strlen(invalid_state_err_msg));
        return -3;
    }

    // change to NOT_SUSPENDED
    pcb->dispatch = NOT_SUSPENDED;
    return 0;
}
    
int show_pcb(const char* name)
{
    //display person's name, class, state, suspended status, priority
    //*Error Checking*
    if(!is_valid_name(name))
    //name must be valid
    {
        return -1;
    }
    
    PCB* pcb = pcb_find(name);
    if(pcb == NULL)
    //name must be found
    {
        char lost_pcb_name[] = "Name is not found\r\n";
        sys_req(WRITE, COM1, lost_pcb_name, strlen(lost_pcb_name));
        return -2;
    }

    //Name process
    char pcb_nm_msg[] = "PCB name: ";
    sys_req(WRITE, COM1, pcb_nm_msg, strlen(pcb_nm_msg));
    sys_req(WRITE, COM1, pcb->name, strlen(pcb->name));
    sys_req(WRITE, COM1, "\r\n", 2);
    
    //Class process
    char class_msg[] = "Class: ";
    sys_req(WRITE, COM1, class_msg, strlen(class_msg));
    if(pcb->class == USER)
    { 
        char class_usr_msg[] = "USER\r\n";
        sys_req(WRITE, COM1, class_usr_msg, strlen(class_usr_msg));
    }
    if(pcb->class == SYSTEM)
    { 
        char class_sys_msg[] = "SYSTEM\r\n";
        sys_req(WRITE, COM1, class_sys_msg, strlen(class_sys_msg));
    }
    //v BOTH OF THESE SAME AS CLASS ^
    //State process (execution)
    char state_msg[] = "State: ";
    sys_req(WRITE, COM1, state_msg, strlen(state_msg));
    if(pcb->execution == READY)
    { 
        char state_rdy_msg[] = "READY\r\n";
        sys_req(WRITE, COM1, state_rdy_msg, strlen(state_rdy_msg));
    }
    if(pcb->execution == RUNNING)
    { 
        char state_run_msg[] = "RUNNING\r\n";
        sys_req(WRITE, COM1, state_run_msg, strlen(state_run_msg));
    }
    if(pcb->execution == BLOCKED)
    { 
        char state_blk_msg[] = "BLOCKED\r\n";
        sys_req(WRITE, COM1, state_blk_msg, strlen(state_blk_msg));
    }

    //Status process (dispatch)
    char status_msg[] = "Status: ";
    sys_req(WRITE, COM1, status_msg, strlen(status_msg));
    if(pcb->dispatch == NOT_SUSPENDED)
    { 
        char status_not_msg[] = "NOT_SUSPENDED\r\n";
        sys_req(WRITE, COM1, status_not_msg, strlen(status_not_msg));
    }
    if(pcb->dispatch == SUSPENDED)
    { 
        char status_sus_msg[] = "SUSPENDED\r\n";
        sys_req(WRITE, COM1, status_sus_msg, strlen(status_sus_msg));
    }

    //Priority process
    char priority_msg[] = "Priority: ";
    sys_req(WRITE, COM1, priority_msg, strlen(priority_msg));
    char priority_value_msg[] = "x\r\n";
    priority_value_msg[0] = pcb->priority + '0';
    sys_req(WRITE, COM1, priority_value_msg, strlen(priority_value_msg));
    return 0;
}

void show_ready(void)
{
    //header message saying this is ready queue
     char ready_hdr[] = "READY QUEUE\r\n";
    sys_req(WRITE, COM1, ready_hdr, strlen(ready_hdr));
    //loop through ready queue, use showpcb() + sys_req(\r\n) on each value, + msg incase queue is empty
    PCB* pcb = ready_queue->head;
    if (pcb == NULL)
    {
        char ready_err_msg[] = "Ready queue is empty\r\n\r\n";
        sys_req(WRITE, COM1, ready_err_msg, strlen(ready_err_msg));
    }
    while (pcb != NULL)
    {
        show_pcb(pcb->name);
        sys_req(WRITE, COM1, "\r\n", 2);
        pcb = pcb->next;
    }
} 

void show_blocked(void)
{
    //header message saying this is blocked queue
    char blocked_hdr[] = "BLOCKED QUEUE\r\n";
    sys_req(WRITE, COM1, blocked_hdr, strlen(blocked_hdr));
    //loop through blocked queue, use showpcb() + sys_req(\r\n) on each value, + msg incase queue is empty
    PCB* pcb = blocked_queue->head;
    if (pcb == NULL)
    {
        char blocked_err_msg[] = "Blocked queue is empty\r\n\r\n";
        sys_req(WRITE, COM1, blocked_err_msg, strlen(blocked_err_msg));
    }
    while (pcb != NULL)
    {
        show_pcb(pcb->name);
        sys_req(WRITE, COM1, "\r\n", 2);
        pcb = pcb->next;
    }
}

void show_all(void)
{
    //both show_ready and show_blocked
    show_ready();
    show_blocked();
}
