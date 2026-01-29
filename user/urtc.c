#include <urtc.h>

void get_time(void)
{
    int hours = get_hours();
    int minutes = get_minutes();
    int seconds = get_seconds();

    char time_buffer[TIME_BUFFER_SIZE];

    time_buffer[0] = (hours / 10) + '0';
    time_buffer[1] = (hours % 10) + '0';
    time_buffer[2] = ':';
    time_buffer[3] = (minutes / 10) + '0';
    time_buffer[4] = (minutes % 10) + '0';
    time_buffer[5] = ':';
    time_buffer[6] = (seconds / 10) + '0';
    time_buffer[7] = (seconds % 10) + '0';
    time_buffer[8] = '\r';
    time_buffer[9] = '\n';
    time_buffer[10] = '\0';

    sys_req(WRITE, COM1, time_buffer, TIME_BUFFER_SIZE);
}

int set_time(int hours, int minutes, int seconds)
{
    bool error = false;

    if (hours < 0 || hours > 23)
    {
        char hr_err_msg[] = "Hours must be between 0 and 23\r\n";
        sys_req(WRITE, COM1, hr_err_msg, strlen(hr_err_msg));
        error = true;
    }

    if (minutes < 0 || minutes > 59)
    {
        char min_err_msg[] = "Minutes must be between 0 and 59\r\n";
        sys_req(WRITE, COM1, min_err_msg, strlen(min_err_msg));
        error = true;
    }

    if (seconds < 0 || seconds > 59)
    {
        char sec_err_msg[] = "Seconds must be between 0 and 59\r\n";
        sys_req(WRITE, COM1, sec_err_msg, strlen(sec_err_msg));
        error = true;
    }

    if (error)
    {
        return -1;
    }

    set_hours(hours);
    set_minutes(minutes);
    set_seconds(seconds);

    return 0;
}

void get_date(void)
{
    int month = get_month();
    int day = get_day();
    int year = get_year();

    char date_buffer[DATE_BUFFER_SIZE];

    date_buffer[0] = (month / 10) + '0';
    date_buffer[1] = (month % 10) + '0';
    date_buffer[2] = '/';
    date_buffer[3] = (day / 10) + '0';
    date_buffer[4] = (day % 10) + '0';
    date_buffer[5] = '/';
    date_buffer[6] = (year / 10) + '0';
    date_buffer[7] = (year % 10) + '0';
    date_buffer[8] = '\r';
    date_buffer[9] = '\n';
    date_buffer[10] = '\0';

    sys_req(WRITE, COM1, date_buffer, DATE_BUFFER_SIZE);
}

int set_date(int month, int day, int year)
{
    bool error = false;

    if (month < 1 || month > 12)
    {
        char month_err_msg[] = "Month must be between 1 and 12\r\n";
        sys_req(WRITE, COM1, month_err_msg, strlen(month_err_msg));
        error = true;
    }

    if (day < 1 || day > 31)
    {
        char day_err_msg[] = "Day must be between 1 and 31\r\n";
        sys_req(WRITE, COM1, day_err_msg, strlen(day_err_msg));
        error = true;
    }

    if (year < 0 || year > 99)
    {
        char year_err_msg[] = "Year must be between 0 and 99\r\n";
        sys_req(WRITE, COM1, year_err_msg, strlen(year_err_msg));
        error = true;
    }

    if (error)
    {
        return -1;
    }

    // Check for valid days in month
    switch (month)
    {
        case APRIL:
        case JUNE:
        case SEPTEMBER:
        case NOVEMBER:
            if (day > 30)
            {
                char day_30_err_msg[] = "The day for the given month has must be between 1 and 30\r\n";
                sys_req(WRITE, COM1, day_30_err_msg, strlen(day_30_err_msg));
                return -1;
            }
            break;

        case FEBRUARY:
            // rtc only stores year as two digits, so only every 4 years needs to be checked
            if (year % 4 == 0)
            {
                if (day > 29)
                {
                    char day_29_err_msg[] = "The day for February in a leap year must be between 1 and 29\r\n";
                    sys_req(WRITE, COM1, day_29_err_msg, strlen(day_29_err_msg));
                    return -1;
                }
            }
            else
            {
                if (day > 28)
                {
                    char day_28_err_msg[] = "The day for February in a non-leap year must be between 1 and 28\r\n";
                    sys_req(WRITE, COM1, day_28_err_msg, strlen(day_28_err_msg));
                    return -1;
                }
            }
            break;
    }

    // prevents rtc from rolling over invalid dates
    set_day(1);

    set_year(year);
    set_month(month);
    set_day(day);

    return 0;
}
