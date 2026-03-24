#ifndef MPX_COMHAND_H
#define MPX_COMHAND_H

#include <string.h>
#include <sys_req.h>
#include <mpx/device.h>
#include <urtc.h>
#include <stdlib.h>
#include <upcb.h>
#include <loadr3.h>
#include <alarm.h>

/**
 @file comhand.h
 @brief Command handler function
*/

#define WHITESPACE_CHARS " \r\n\t"
#define BUFFER_SIZE 100

/**
 User help command
*/
void cmd_help(void);

/**
 User version command
*/
void cmd_version(void);

/**
 Command handler loop to handle user commands
*/
void comhand(void);

#endif
