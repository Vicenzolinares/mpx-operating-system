#ifndef MPX_KRTC_H
#define MPX_KRTC_H

#include <mpx/io.h>
#include <mpx/interrupts.h>

/**
 @file krtc.h
 @brief Kernel-level RTC functions
*/


// Read from RTC functions

/**
 Convert a binary coded decimal (BCD) value to an integer
 @param bcd The BCD value to convert (1 byte)
 @return The integer representation of the BCD value
*/
int bcd_to_int(unsigned char bcd);

/**
 Get the current seconds from the RTC
 @return The current seconds
*/
int get_seconds(void);

/**
 Get the current minutes from the RTC
 @return The current minutes
*/
int get_minutes(void);

/**
 Get the current hours from the RTC
 @return The current hours
*/
int get_hours(void);

/**
 Get the current day from the RTC
 @return The current day
*/
int get_day(void);

/**
 Get the current month from the RTC
 @return The current month
*/
int get_month(void);

/**
 Get the current year from the RTC
 @return The current year
*/
int get_year(void);


// Write to RTC functions

/**
 Convert an integer value to a binary coded decimal (BCD) value
 @param value The integer value to convert
 @return The BCD representation of the integer value
*/
unsigned char int_to_bcd(int value);

/**
 Set the current seconds in the RTC
 @param seconds The seconds to set
*/
void set_seconds(int seconds);

/**
 Set the current minutes in the RTC
 @param minutes The minutes to set
*/
void set_minutes(int minutes);

/**
 Set the current hours in the RTC
 @param hours The hours to set
*/
void set_hours(int hours);

/**
 Set the current day in the RTC
 @param day The day to set
*/
void set_day(int day);

/**
 Set the current month in the RTC
 @param month The month to set
*/
void set_month(int month);

/**
 Set the current year in the RTC
 @param year The year to set
*/
void set_year(int year);

#endif
