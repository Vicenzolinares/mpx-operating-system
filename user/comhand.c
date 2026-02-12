#include <comhand.h>


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

		// unknown command
		else
		{
			mpx_write("Unknown command.\r\n");
		}
	}
}
