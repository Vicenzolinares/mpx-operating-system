#ifndef COMHAND_H
#define COMHAND_H

#include <string.h>
#include <sys_req.h>
#include <mpx/device.h>
#include <urtc.h>

#define WHITESPACE_CHARS " \r\n\t"
#define BUFFER_SIZE 100

void cmd_help(void);
void cmd_version(void);
void comhand(void);

#endif
