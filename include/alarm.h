#ifndef MPX_ALARM_H
#define MPX_ALARM_H

#include <stdbool.h>
#include <comhand.h>
#include <ctype.h>
#include <sys_req.h>
#include <string.h>
#include <upcb.h>
#include <mpx/kpcb.h>

/**
 @file alarm.h
 @brief Alarm functions
*/

#define ALARM_PROCESS_LIMIT 10

// buffer size from comhand
#define BUFFER_SIZE 100

typedef struct alarm_request {
	bool active;
	int hours;
	int minutes;
	int seconds;
    int start;
	char message[BUFFER_SIZE];
} alarm_request;

/**
 Validates the inputted time
 @param hours The hours for the alarm
 @param minutes The minutes for the alarm
 @param seconds The seconds for the alarm
 @return true if the time is valid, false otherwise
*/
bool valid_alarm_time(int hours, int minutes, int seconds);

// Alarm functions given to pcb context
void alarm_process_0(void);
void alarm_process_1(void);
void alarm_process_2(void);
void alarm_process_3(void);
void alarm_process_4(void);
void alarm_process_5(void);
void alarm_process_6(void);
void alarm_process_7(void);
void alarm_process_8(void);
void alarm_process_9(void);

/**
 Creates an alarm process with the specified time and message
 @param hours The hours for the alarm
 @param minutes The minutes for the alarm
 @param seconds The seconds for the alarm
 @param message The message to display when the alarm goes off
 @return 0 on success, -1 if no alarm slots are available or PCB creation fails
*/
int create_alarm_process(int hours, int minutes, int seconds, const char *message);

#endif
