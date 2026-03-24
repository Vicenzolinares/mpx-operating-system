#include <comhand.h>
#include <upcb.h>
#include <loadr3.h>

#define ALARM_PROCESS_LIMIT 10

typedef struct alarm_request {
	bool active;
	int hours;
	int minutes;
	int seconds;
	char message[BUFFER_SIZE];
} alarm_request;

static alarm_request alarm_requests[ALARM_PROCESS_LIMIT] = { 0 };

static int is_whitespace_char(char c)
{
	return c == ' ' || c == '\r' || c == '\n' || c == '\t';
}

static int parse_alarm_time(const char *token, int *hours, int *minutes, int *seconds)
{
	int values[3] = { 0 };
	const char *ptr = token;

	if (token == NULL)
	{
		return 0;
	}

	for (int index = 0; index < 3; index++)
	{
		int digits = 0;
		int value = 0;

		while (*ptr >= '0' && *ptr <= '9')
		{
			value = (value * 10) + (*ptr - '0');
			ptr++;
			digits++;

			if (digits > 2)
			{
				return 0;
			}
		}

		if (digits == 0)
		{
			return 0;
		}

		values[index] = value;

		if (index < 2)
		{
			if (*ptr != ':')
			{
				return 0;
			}

			ptr++;
		}
	}

	if (*ptr != '\0')
	{
		return 0;
	}

	if (values[0] < 0 || values[0] > 23 || values[1] < 0 || values[1] > 59 || values[2] < 0 || values[2] > 59)
	{
		return 0;
	}

	*hours = values[0];
	*minutes = values[1];
	*seconds = values[2];
	return 1;
}

static char *get_alarm_message(char *input)
{
	char *ptr = input;
	char *end;

	while (is_whitespace_char(*ptr))
	{
		ptr++;
	}

	while (*ptr != '\0' && !is_whitespace_char(*ptr))
	{
		ptr++;
	}

	while (is_whitespace_char(*ptr))
	{
		ptr++;
	}

	while (*ptr != '\0' && !is_whitespace_char(*ptr))
	{
		ptr++;
	}

	while (is_whitespace_char(*ptr))
	{
		ptr++;
	}

	if (*ptr == '\0')
	{
		return NULL;
	}

	end = ptr + strlen(ptr);
	while (end > ptr && is_whitespace_char(*(end - 1)))
	{
		end--;
	}

	*end = '\0';
	return ptr;
}

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

static void initialize_process_context(PCB *pcb, void (*entry)(void))
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
	context->eip = (intptr_t) entry;
	context->eflags = 0x0202;
	context->eax = 0;
	context->ebx = 0;
	context->ecx = 0;
	context->edx = 0;
	context->esi = 0;
	context->edi = 0;
}

static int time_to_seconds(int hours, int minutes, int seconds)
{
	return (hours * 3600) + (minutes * 60) + seconds;
}

static void run_alarm_process(int slot)
{
	alarm_request *alarm = &alarm_requests[slot];
	int target_seconds = time_to_seconds(alarm->hours, alarm->minutes, alarm->seconds);
	int current_seconds = time_to_seconds(get_hours(), get_minutes(), get_seconds());

	while (current_seconds < target_seconds)
	{
		sys_req(IDLE);
		current_seconds = time_to_seconds(get_hours(), get_minutes(), get_seconds());
	}

	mpx_write("Alarm: ");
	mpx_write(alarm->message);
	mpx_write("\r\n");
	alarm->active = false;
	sys_req(EXIT);

	for (;;)
	{
		sys_req(EXIT);
	}
}

#define DEFINE_ALARM_PROCESS(slot) \
	static void alarm_process_##slot(void) \
	{ \
		run_alarm_process(slot); \
	}

DEFINE_ALARM_PROCESS(0)
DEFINE_ALARM_PROCESS(1)
DEFINE_ALARM_PROCESS(2)
DEFINE_ALARM_PROCESS(3)
DEFINE_ALARM_PROCESS(4)
DEFINE_ALARM_PROCESS(5)
DEFINE_ALARM_PROCESS(6)
DEFINE_ALARM_PROCESS(7)
DEFINE_ALARM_PROCESS(8)
DEFINE_ALARM_PROCESS(9)

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

static int create_alarm_process(int hours, int minutes, int seconds, const char *message)
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
		mpx_write("Error: No alarm slots available.\r\n");
		return -1;
	}

	alarm_requests[slot].active = true;
	alarm_requests[slot].hours = hours;
	alarm_requests[slot].minutes = minutes;
	alarm_requests[slot].seconds = seconds;
	memcpy(alarm_requests[slot].message, message, strlen(message) + 1);

	build_alarm_name(slot, name);
	if (create_pcb(name, USER, 5) != 0)
	{
		alarm_requests[slot].active = false;
		return -1;
	}

	pcb = pcb_find(name);
	if (pcb == NULL)
	{
		alarm_requests[slot].active = false;
		mpx_write("Failed to create PCB\r\n");
		return -1;
	}

	initialize_process_context(pcb, alarm_process_entries[slot]);
	return 0;
}

/**
 Helper function to write a string to COM1
 @param s The string to write
*/
static void mpx_write(const char *s)
{
	if (s == NULL)
		return;

	sys_req(WRITE, COM1, (char *)s, strlen(s));
}

void cmd_help(void)
{
	mpx_write("Commands:\r\n");
    mpx_write("  date get - Displays the current system date\r\n");
    mpx_write("  date set <month> <day> <year> - Sets the system date mm dd yy\r\n");
	mpx_write("  alarm <hh:mm:ss> <message> - Creates an alarm process\r\n");
    mpx_write("  help - Shows this list of available commands\r\n");
    mpx_write("  shutdown - Shuts down the system and prompts for the confirmation\r\n");
    mpx_write("  time get - Displays the current system time [in UTC time]\r\n");
    mpx_write("  time set <hour> <minute> <second> - Sets the system time hh mm ss [in UTC time]\r\n");
    mpx_write("  version - Shows build/version information\r\n");
    mpx_write("\r\n");
    mpx_write("pcb Process Commands:\r\n");
    mpx_write("  pcb delete <name> - Delete a non-system process | <name> 1-8 characters\r\n");
    mpx_write("  pcb block <name> - Block a non-system process | <name> 1-8 characters\r\n");
    mpx_write("  pcb unblock <name> - Unblock a process | <name> 1-8 characters\r\n");
    mpx_write("  pcb suspend <name> - Suspend a non-system process | <name> 1-8 characters\r\n");
    mpx_write("  pcb resume <name> - Resume a process | <name> 1-8 characters\r\n");
    mpx_write("  pcb set priority <name> <priority> - Set non-system process priority | <name> 1-8 characters | <priority> 0-9\r\n");
    mpx_write("  pcb show name <name> - Show process information | <name> 1-8 characters\r\n");
    mpx_write("  pcb show ready - Show all ready processes' information\r\n");
    mpx_write("  pcb show blocked - Show all blocked processes' information\r\n");
    mpx_write("  pcb show all - Show all processes' information\r\n");
	mpx_write("\r\n");
	mpx_write("R3 Process Commands:\r\n");
	mpx_write("  loadprocess <proc1|proc2|proc3|proc4|proc5> - Load one R3 process from proc1 to proc5\r\n");
	mpx_write("  loadr3 - Load all R3 processes as ready, non-suspended\r\n");
	mpx_write("  loadr3suspended - Load all R3 processes as ready, suspended\r\n");
	mpx_write("  resume all - Resume all ready, suspended processes\r\n");
}

void cmd_version(void)
{
	mpx_write("MPX R4\r\n");
	mpx_write("Build: ");
	mpx_write(__DATE__);
	mpx_write(" ");
	mpx_write(__TIME__);
	mpx_write("\r\n");
}

void comhand(void)
{
	for (;;)
	{
		sys_req(IDLE);
		mpx_write("mpx> ");
		char buf[BUFFER_SIZE] = { 0 };
		char raw_buf[BUFFER_SIZE] = { 0 };
		sys_req(READ, COM1, buf, BUFFER_SIZE);
		memcpy(raw_buf, buf, BUFFER_SIZE);

		char *parse_string_ptr = strtok(buf, WHITESPACE_CHARS);

		// no command
		if (parse_string_ptr == NULL)
		{
			// new line for empty command
		}

		// help command
		else if (strcmp(parse_string_ptr, "help") == 0)
		{
			parse_string_ptr = strtok(NULL, WHITESPACE_CHARS);
			if (parse_string_ptr != NULL)
			{
				mpx_write("Error: 'help' command takes no arguments.\r\n");
				continue;
			}
			cmd_help();
		}

		// version command
		else if (strcmp(parse_string_ptr, "version") == 0)
		{
			parse_string_ptr = strtok(NULL, WHITESPACE_CHARS);
			if (parse_string_ptr != NULL)
			{
				mpx_write("Error: 'version' command takes no arguments.\r\n");
				continue;
			}
			cmd_version();
		}

		// date command
		else if (strcmp(parse_string_ptr, "date") == 0)
		{
			parse_string_ptr = strtok(NULL, WHITESPACE_CHARS);
			if (parse_string_ptr == NULL)
			{
				mpx_write("Error: 'date' get or set.\r\n");
				continue;
			}

			// date get subcommand
			if (strcmp(parse_string_ptr, "get") == 0)
			{
				parse_string_ptr = strtok(NULL, WHITESPACE_CHARS);
				if (parse_string_ptr != NULL)
				{
					mpx_write("Error: 'date get' command takes no arguments.\r\n");
					continue;
				}

				get_date();
			}

			// date set subcommand
			else if (strcmp(parse_string_ptr, "set") == 0)
			{
				int month, day, year;

				parse_string_ptr = strtok(NULL, WHITESPACE_CHARS);
				if (parse_string_ptr == NULL)
				{
					mpx_write("Error: 'date set' command takes 3 arguments.\r\n");
					continue;
				}

				month = atoi(parse_string_ptr);

				parse_string_ptr = strtok(NULL, WHITESPACE_CHARS);
				if (parse_string_ptr == NULL)
				{
					mpx_write("Error: 'date set' command takes 3 arguments.\r\n");
					continue;
				}

				day = atoi(parse_string_ptr);

				parse_string_ptr = strtok(NULL, WHITESPACE_CHARS);
				if (parse_string_ptr == NULL)
				{
					mpx_write("Error: 'date set' command takes 3 arguments.\r\n");
					continue;
				}

				year = atoi(parse_string_ptr);

				parse_string_ptr = strtok(NULL, WHITESPACE_CHARS);
				if (parse_string_ptr != NULL)
				{
					mpx_write("Error: 'date set' command takes 3 arguments.\r\n");
					continue;
				}

				if (set_date(month, day, year) == 0)
				{
					mpx_write("Date set to: ");
					get_date();
				}
			}
			else
			{
				mpx_write("Error: Unknown subcommand for 'date'.\r\n");
			}
		}

		// time command
		else if (strcmp(parse_string_ptr, "time") == 0)
		{
			parse_string_ptr = strtok(NULL, WHITESPACE_CHARS);
			if (parse_string_ptr == NULL)
			{
				mpx_write("Error: 'time' get or set.\r\n");
				continue;
			}

			// time get subcommand
			if (strcmp(parse_string_ptr, "get") == 0)
			{
				parse_string_ptr = strtok(NULL, WHITESPACE_CHARS);
				if (parse_string_ptr != NULL)
				{
					mpx_write("Error: 'time get' command takes no arguments.\r\n");
					continue;
				}

				get_time();
			}

			// time set subcommand
			else if (strcmp(parse_string_ptr, "set") == 0)
			{
				int hour, minute, second;

				parse_string_ptr = strtok(NULL, WHITESPACE_CHARS);
				if (parse_string_ptr == NULL)
				{
					mpx_write("Error: 'time set' command takes 3 arguments.\r\n");
					continue;
				}

				hour = atoi(parse_string_ptr);

				parse_string_ptr = strtok(NULL, WHITESPACE_CHARS);
				if (parse_string_ptr == NULL)
				{
					mpx_write("Error: 'time set' command takes 3 arguments.\r\n");
					continue;
				}

				minute = atoi(parse_string_ptr);

				parse_string_ptr = strtok(NULL, WHITESPACE_CHARS);
				if (parse_string_ptr == NULL)
				{
					mpx_write("Error: 'time set' command takes 3 arguments.\r\n");
					continue;
				}

				second = atoi(parse_string_ptr);

				parse_string_ptr = strtok(NULL, WHITESPACE_CHARS);
				if (parse_string_ptr != NULL)
				{
					mpx_write("Error: 'time set' command takes 3 arguments.\r\n");
					continue;
				}

				if (set_time(hour, minute, second) == 0)
				{
					mpx_write("Time set to: ");
					get_time();
				}
			}
			else
			{
				mpx_write("Error: Unknown subcommand for 'time'.\r\n");
			}
		}

		// shutdown command
		else if (strcmp(parse_string_ptr, "shutdown") == 0)
		{
			parse_string_ptr = strtok(NULL, WHITESPACE_CHARS);
			if (parse_string_ptr != NULL)
			{
				mpx_write("Error: 'shutdown' command takes no arguments.\r\n");
				continue;
			}

			mpx_write("Confirm: Y/N\r\n");
			sys_req(READ, COM1, buf, BUFFER_SIZE);

			parse_string_ptr = strtok(buf, WHITESPACE_CHARS);
			if (parse_string_ptr != NULL && (strcmp(parse_string_ptr, "Y") == 0 || strcmp(parse_string_ptr, "y") == 0))
			{
				mpx_write("Shutting down...\r\n");
				sys_req(EXIT);

				for (;;)
				{
					sys_req(EXIT);
				}
			}
			else
			{
				mpx_write("Shutdown cancelled.\r\n");
			}
		}

		// alarm command
		else if (strcmp(parse_string_ptr, "alarm") == 0)
		{
			char *time_token;
			char *message;
			int hours;
			int minutes;
			int seconds;

			time_token = strtok(NULL, WHITESPACE_CHARS);
			if (time_token == NULL)
			{
				mpx_write("Error: 'alarm' command requires a time and message.\r\n");
				continue;
			}

			message = get_alarm_message(raw_buf);
			if (message == NULL)
			{
				mpx_write("Error: 'alarm' command requires a time and message.\r\n");
				continue;
			}

			if (!parse_alarm_time(time_token, &hours, &minutes, &seconds))
			{
				mpx_write("Error: Invalid alarm time. Use hh:mm:ss.\r\n");
				continue;
			}

			if (create_alarm_process(hours, minutes, seconds, message) == 0)
			{
				mpx_write("Alarm created.\r\n");
			}
		}

		// loadr3 command
		else if (strcmp(parse_string_ptr, "loadr3") == 0)
		{
			parse_string_ptr = strtok(NULL, WHITESPACE_CHARS);
			if (parse_string_ptr != NULL)
			{
				mpx_write("Error: 'loadr3' command takes no arguments.\r\n");
				continue;
			}

			load_R3();
		}

		// loadr3suspended command
		else if (strcmp(parse_string_ptr, "loadr3suspended") == 0)
		{
			parse_string_ptr = strtok(NULL, WHITESPACE_CHARS);
			if (parse_string_ptr != NULL)
			{
				mpx_write("Error: 'loadr3suspended' command takes no arguments.\r\n");
				continue;
			}

			load_R3_suspended();
		}

		// loadprocess <proc1..proc5>
		else if (strcmp(parse_string_ptr, "loadprocess") == 0)
		{
			char *name = strtok(NULL, WHITESPACE_CHARS);

			if (name == NULL)
			{
				mpx_write("Error: 'loadprocess' command takes 1 argument.\r\n");
				continue;
			}

			parse_string_ptr = strtok(NULL, WHITESPACE_CHARS);
			if (parse_string_ptr != NULL)
			{
				mpx_write("Error: 'loadprocess' command takes 1 argument.\r\n");
				continue;
			}

			load_process(name, NOT_SUSPENDED);
		}

		// resume all
		else if (strcmp(parse_string_ptr, "resume") == 0)
		{
			char *target = strtok(NULL, WHITESPACE_CHARS);

			if (target == NULL || strcmp(target, "all") != 0)
			{
				mpx_write("Error: Use 'resume all'.\r\n");
				continue;
			}

			parse_string_ptr = strtok(NULL, WHITESPACE_CHARS);
			if (parse_string_ptr != NULL)
			{
				mpx_write("Error: 'resume all' command takes no additional arguments.\r\n");
				continue;
			}

			resume_all();
		}

		// pcb command
		else if (strcmp(parse_string_ptr, "pcb") == 0)
		{
			parse_string_ptr = strtok(NULL, WHITESPACE_CHARS);
			if (parse_string_ptr == NULL)
			{
				mpx_write("Error: Missing pcb subcommand.\r\n");
				continue;
			}

			// pcb delete <name>
			else if (strcmp(parse_string_ptr, "delete") == 0)
			{
				char *name = strtok(NULL, WHITESPACE_CHARS);

				if (name == NULL)
				{
					mpx_write("Error: 'pcb delete' command takes 1 argument.\r\n");
					continue;
				}

				parse_string_ptr = strtok(NULL, WHITESPACE_CHARS);
				if (parse_string_ptr != NULL)
				{
					mpx_write("Error: 'pcb delete' command takes 1 argument.\r\n");
					continue;
				}

				if (delete_pcb(name) == 0)
				{
					mpx_write("PCB deleted.\r\n");
				}
			}

			// pcb block <name>
			else if (strcmp(parse_string_ptr, "block") == 0)
			{
				char *name = strtok(NULL, WHITESPACE_CHARS);

				if (name == NULL)
				{
					mpx_write("Error: 'pcb block' command takes 1 argument.\r\n");
					continue;
				}

				parse_string_ptr = strtok(NULL, WHITESPACE_CHARS);
				if (parse_string_ptr != NULL)
				{
					mpx_write("Error: 'pcb block' command takes 1 argument.\r\n");
					continue;
				}

				if (block_pcb(name) == 0)
				{
					mpx_write("PCB blocked.\r\n");
				}
			}

			// pcb unblock <name>
			else if (strcmp(parse_string_ptr, "unblock") == 0)
			{
				char *name = strtok(NULL, WHITESPACE_CHARS);

				if (name == NULL)
				{
					mpx_write("Error: 'pcb unblock' command takes 1 argument.\r\n");
					continue;
				}

				parse_string_ptr = strtok(NULL, WHITESPACE_CHARS);
				if (parse_string_ptr != NULL)
				{
					mpx_write("Error: 'pcb unblock' command takes 1 argument.\r\n");
					continue;
				}

				if (unblock_pcb(name) == 0)
				{
					mpx_write("PCB unblocked.\r\n");
				}
			}

			// pcb suspend <name>
			else if (strcmp(parse_string_ptr, "suspend") == 0)
			{
				char *name = strtok(NULL, WHITESPACE_CHARS);

				if (name == NULL)
				{
					mpx_write("Error: 'pcb suspend' command takes 1 argument.\r\n");
					continue;
				}

				parse_string_ptr = strtok(NULL, WHITESPACE_CHARS);
				if (parse_string_ptr != NULL)
				{
					mpx_write("Error: 'pcb suspend' command takes 1 argument.\r\n");
					continue;
				}

				if (suspend_pcb(name) == 0)
				{
					mpx_write("PCB suspended.\r\n");
				}
			}

			// pcb resume <name>
			else if (strcmp(parse_string_ptr, "resume") == 0)
			{
				char *name = strtok(NULL, WHITESPACE_CHARS);

				if (name == NULL)
				{
					mpx_write("Error: 'pcb resume' command takes 1 argument.\r\n");
					continue;
				}

				parse_string_ptr = strtok(NULL, WHITESPACE_CHARS);
				if (parse_string_ptr != NULL)
				{
					mpx_write("Error: 'pcb resume' command takes 1 argument.\r\n");
					continue;
				}

				if (resume_pcb(name) == 0)
				{
					mpx_write("PCB resumed.\r\n");
				}
			}

			// pcb set priority <name> <priority>
			else if (strcmp(parse_string_ptr, "set") == 0)
			{
				char *field = strtok(NULL, WHITESPACE_CHARS);
				char *name = strtok(NULL, WHITESPACE_CHARS);
				char *priority_token = strtok(NULL, WHITESPACE_CHARS);
				int priority_value;

				if (field == NULL || strcmp(field, "priority") != 0)
				{
					mpx_write("Error: Use 'pcb set priority <name> <priority>'.\r\n");
					continue;
				}

				if (name == NULL || priority_token == NULL)
				{
					mpx_write("Error: 'pcb set priority' command takes 2 arguments.\r\n");
					continue;
				}

				parse_string_ptr = strtok(NULL, WHITESPACE_CHARS);
				if (parse_string_ptr != NULL)
				{
					mpx_write("Error: 'pcb set priority' command takes 2 arguments.\r\n");
					continue;
				}

				priority_value = atoi(priority_token);

				if (set_pcb_priority(name, priority_value) == 0)
				{
					mpx_write("PCB priority updated.\r\n");
				}
			}

			// pcb show <name> | pcb show ready | pcb show blocked | pcb show all | pcb show suspended
			else if (strcmp(parse_string_ptr, "show") == 0)
			{
				char *target = strtok(NULL, WHITESPACE_CHARS);

				if (target == NULL)
				{
					mpx_write("Error: 'pcb show' requires a name or queue selector.\r\n");
					continue;
				}

				parse_string_ptr = strtok(NULL, WHITESPACE_CHARS);
				if (parse_string_ptr != NULL)
				{
					mpx_write("Error: 'pcb show' command has too many arguments.\r\n");
					continue;
				}

				if (strcmp(target, "ready") == 0)
				{
					show_ready();
				}
				else if (strcmp(target, "blocked") == 0)
				{
					show_blocked();
				}
				else if (strcmp(target, "all") == 0)
				{
					show_all();
				}
				else
				{
					show_pcb(target);
				}
			}

			else
			{
				mpx_write("Error: Unknown subcommand for 'pcb'.\r\n");
			}
		}
		// unknown command
		else
		{
			mpx_write("Unknown command.\r\n");
		}
	}
}
