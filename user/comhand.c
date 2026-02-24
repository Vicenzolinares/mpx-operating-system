#include <comhand.h>
#include <upcb.h>


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
    mpx_write("  help - Shows this list of available commands\r\n");
    mpx_write("  shutdown - Shuts down the system and prompts for the confirmation\r\n");
    mpx_write("  time get - Displays the current system time [in UTC time]\r\n");
    mpx_write("  time set <hour> <minute> <second> - Sets the system time hh mm ss [in UTC time]\r\n");
    mpx_write("  version - Shows build/version information\r\n");
    mpx_write("\r\n");
    mpx_write("pcb Process Commands:\r\n");
    mpx_write("  pcb create <name> <class> <priority> - Create a new process\r\n");
    mpx_write("  pcb delete <name> - Delete a process\r\n");
    mpx_write("  pcb block <name> - Block a process\r\n");
    mpx_write("  pcb unblock <name> - Unblock a process\r\n");
    mpx_write("  pcb suspend <name> - Suspend a process\r\n");
    mpx_write("  pcb resume <name> - Resume a process\r\n");
    mpx_write("  pcb set priority <name> <priority> - Set process priority\r\n");
    mpx_write("  pcb show <name> - Show process information\r\n");
    mpx_write("  pcb show ready - Show all ready processes\r\n");
    mpx_write("  pcb show blocked - Show all blocked processes\r\n");
    mpx_write("  pcb show all - Show all processes\r\n");
}

void cmd_version(void)
{
	mpx_write("MPX R2\r\n");
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
		mpx_write("mpx> ");
		char buf[BUFFER_SIZE] = { 0 };
		sys_req(READ, COM1, buf, BUFFER_SIZE);

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
				return;
			}
			else
			{
				mpx_write("Shutdown cancelled.\r\n");
			}
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

			// pcb create <name> <class> <priority>
			if (strcmp(parse_string_ptr, "create") == 0)
			{
				char *name = strtok(NULL, WHITESPACE_CHARS);
				char *class_token = strtok(NULL, WHITESPACE_CHARS);
				char *priority_token = strtok(NULL, WHITESPACE_CHARS);
				PCB_class class_value;
				int priority_value;

				if (name == NULL || class_token == NULL || priority_token == NULL)
				{
					mpx_write("Error: 'pcb create' command takes 3 arguments.\r\n");
					continue;
				}

				parse_string_ptr = strtok(NULL, WHITESPACE_CHARS);
				if (parse_string_ptr != NULL)
				{
					mpx_write("Error: 'pcb create' command takes 3 arguments.\r\n");
					continue;
				}

				if (strcmp(class_token, "user") == 0 || strcmp(class_token, "0") == 0)
				{
					class_value = USER;
				}
				else if (strcmp(class_token, "system") == 0 || strcmp(class_token, "1") == 0)
				{
					class_value = SYSTEM;
				}
				else
				{
					mpx_write("Error: class must be 'user'/'system' or 0/1.\r\n");
					continue;
				}

				priority_value = atoi(priority_token);

				if (create_pcb(name, class_value, priority_value) == 0)
				{
					mpx_write("PCB created.\r\n");
				}
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
