#ifndef MPX_URTC_H
#define MPX_URTC_H

#include <mpx/krtc.h>
#include <sys_req.h>
#include <string.h>
#include <stdbool.h>

/**
 @file urtc.h
 @brief User-level RTC functions
*/

#define TIME_BUFFER_SIZE 11
#define DATE_BUFFER_SIZE 11

enum Month {
    JANUARY = 1,
    FEBRUARY,
    MARCH,
    APRIL,
    MAY,
    JUNE,
    JULY,
    AUGUST,
    SEPTEMBER,
    OCTOBER,
    NOVEMBER,
    DECEMBER
};

/**
 Get the current time from the RTC and print it to COM1
*/
void get_time(void);

/**
 Set the current time in the RTC, at UTC time
 @param hours The hours to set (0-23)
 @param minutes The minutes to set (0-59)
 @param seconds The seconds to set (0-59)
 @return 0 on success, -1 on error
*/
int set_time(int hours, int minutes, int seconds);

/**
 Get the current date from the RTC and print it to COM1
*/
void get_date(void);

/**
 Set the current date in the RTC
 @param month The month to set (1-12)
 @param day The day to set (1-31) varies by month
 @param year The year to set (0-99)
 @return 0 on success, -1 on error
*/
int set_date(int month, int day, int year);

#endif
