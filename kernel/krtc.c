#include <mpx/krtc.h>

// Read from RTC functions
int bcd_to_int(unsigned char bcd)
{
    return (int)(((bcd >> 4) * 10) + (bcd & 0x0F));
}

int get_seconds(void)
{
    outb(0x70, 0x00);
    unsigned char seconds = inb(0x71);
    return bcd_to_int(seconds);
}

int get_minutes(void)
{
    outb(0x70, 0x02);
    unsigned char minutes = inb(0x71);
    return bcd_to_int(minutes);
}

int get_hours(void)
{
    outb(0x70, 0x04);
    unsigned char hours = inb(0x71);
    return bcd_to_int(hours);
}

int get_day(void)
{
    outb(0x70, 0x07);
    unsigned char day = inb(0x71);
    return bcd_to_int(day);
}

int get_month(void)
{
    outb(0x70, 0x08);
    unsigned char month = inb(0x71);
    return bcd_to_int(month);
}

int get_year(void)
{
    outb(0x70, 0x09);
    unsigned char year = inb(0x71);
    return bcd_to_int(year);
}

// Write to RTC functions
unsigned char int_to_bcd(int value)
{
    return (unsigned char)(((value / 10) << 4) | (value % 10));
}

void set_seconds(int seconds)
{
    cli();
    outb(0x70, 0x00);
    outb(0x71, int_to_bcd(seconds));
    sti();
}

void set_minutes(int minutes)
{
    cli();
    outb(0x70, 0x02);
    outb(0x71, int_to_bcd(minutes));
    sti();
}

void set_hours(int hours)
{
    cli();
    outb(0x70, 0x04);
    outb(0x71, int_to_bcd(hours));
    sti();
}

void set_day(int day)
{
    cli();
    outb(0x70, 0x07);
    outb(0x71, int_to_bcd(day));
    sti();
}

void set_month(int month)
{
    cli();
    outb(0x70, 0x08);
    outb(0x71, int_to_bcd(month));
    sti();
}

void set_year(int year)
{
    cli();
    outb(0x70, 0x09);
    outb(0x71, int_to_bcd(year));
    sti();
}
