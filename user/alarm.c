#include <alarm.h>

static alarm_request alarm_requests[ALARM_PROCESS_LIMIT] = { 0 };

bool valid_alarm_time(int hours, int minutes, int seconds)
{
    if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59 || seconds < 0 || seconds > 59)
    {
        return false;
    }
    return true;
}

/**
 Helper function to make the alarm process name based on the slot number
 @param slot The alarm slot number
 @param name Pointer to where the name will be made
*/
static void build_alarm_name(int slot, char *name)
{
	name[0] = 'a';
	name[1] = 'l';
	name[2] = 'a';
	name[3] = 'r';
	name[4] = 'm';
	name[5] = (char)('0' + slot);
	name[6] = '\0';
}

/**
 Initializes the process context for an alarm process
 @param pcb Pointer to the PCB
 @param entry The function the process will execute
*/
static void initialize_process_context(PCB *pcb, int entry)
{
	struct context *context;

	pcb->stack_pointer = pcb->stack_pointer - sizeof(struct context);
	context = (struct context *) pcb->stack_pointer;
	context->cs = 0x08;
	context->ds = 0x10;
	context->es = 0x10;
	context->fs = 0x10;
	context->gs = 0x10;
	context->ss = 0x10;
	context->ebp = (int) pcb->stack;
	context->eip = entry;
	context->eflags = 0x0202;
	context->eax = 0;
	context->ebx = 0;
	context->ecx = 0;
	context->edx = 0;
	context->esi = 0;
	context->edi = 0;
}

/**
 Converts a time to seconds
 @param hours The hours
 @param minutes The minutes
 @param seconds The seconds
 @return The total seconds
*/
static int time_to_seconds(int hours, int minutes, int seconds)
{
	return (hours * 3600) + (minutes * 60) + seconds;
}

/**
 Runs the alarm process for the specified slot
 @param slot The alarm slot number
*/
static void run_alarm_process(int slot)
{
	alarm_request *alarm = &alarm_requests[slot];
	int target_seconds = time_to_seconds(alarm->hours, alarm->minutes, alarm->seconds);
	int current_seconds = time_to_seconds(get_hours(), get_minutes(), get_seconds());

    for(;;)
    {
        if (target_seconds < alarm->start)
        {
            if (current_seconds >= alarm->start || current_seconds < target_seconds)
            {
                sys_req(IDLE);
                current_seconds = time_to_seconds(get_hours(), get_minutes(), get_seconds());
                continue;
            }
            break;
        }
        else
        {
            if (current_seconds < target_seconds)
            {
                sys_req(IDLE);
                current_seconds = time_to_seconds(get_hours(), get_minutes(), get_seconds());
                continue;
            }
            break;
        }
    }

	char alarm_start_msg[] = "Alarm: ";
    sys_req(WRITE, COM1, alarm_start_msg, strlen(alarm_start_msg));
    sys_req(WRITE, COM1, alarm->message, strlen(alarm->message));
    sys_req(WRITE, COM1, "\r\n", 2);
	alarm->active = false;
	sys_req(EXIT);
}

// Array of function pointers for all alarm slots
static void (*const alarm_process_entries[ALARM_PROCESS_LIMIT])(void) = {
	alarm_process_0,
	alarm_process_1,
	alarm_process_2,
	alarm_process_3,
	alarm_process_4,
	alarm_process_5,
	alarm_process_6,
	alarm_process_7,
	alarm_process_8,
	alarm_process_9,
};

// Alarm process functions
void alarm_process_0(void)
{
	run_alarm_process(0);
}
void alarm_process_1(void)
{
	run_alarm_process(1);
}
void alarm_process_2(void)
{
	run_alarm_process(2);
}
void alarm_process_3(void)
{
	run_alarm_process(3);
}
void alarm_process_4(void)
{
	run_alarm_process(4);
}
void alarm_process_5(void)
{
	run_alarm_process(5);
}
void alarm_process_6(void)
{
	run_alarm_process(6);
}
void alarm_process_7(void)
{
	run_alarm_process(7);
}
void alarm_process_8(void)
{
	run_alarm_process(8);
}
void alarm_process_9(void)
{
	run_alarm_process(9);
}

int create_alarm_process(int hours, int minutes, int seconds, const char *message)
{
	char name[PCB_NAME_LENGTH + 1];
	int slot;
	PCB *pcb;

	for (slot = 0; slot < ALARM_PROCESS_LIMIT; slot++)
	{
		if (!alarm_requests[slot].active)
		{
			break;
		}
	}

	if (slot == ALARM_PROCESS_LIMIT)
	{
        char alarm_limit_msg[] = "Error: No alarm slots available.\r\n";
		sys_req(WRITE, COM1, alarm_limit_msg, sizeof(alarm_limit_msg));
        return -1;
	}

	alarm_requests[slot].active = true;
	alarm_requests[slot].hours = hours;
	alarm_requests[slot].minutes = minutes;
	alarm_requests[slot].seconds = seconds;
    alarm_requests[slot].start = time_to_seconds(get_hours(), get_minutes(), get_seconds());
	memcpy(alarm_requests[slot].message, message, strlen(message) + 1);

	build_alarm_name(slot, name);
	if (create_pcb(name, USER, 5) != 0)
	{
		alarm_requests[slot].active = false;
		return -1;
	}

	pcb = pcb_find(name);

	initialize_process_context(pcb, (int) alarm_process_entries[slot]);
	return 0;
}
