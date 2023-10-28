#ifndef RTC_H
#define RTC_H

#include <stdint.h>
#include <stddef.h>
// #include <cpu/io.h>

typedef struct poww_time {
    int sec;      // [0, 59]
    int min;      // [0, 59]
    int hour;     // [0, 23]
    int day;      // [1, 31]
    int month;    // [1, 12]
    int year;     // [1970, 2038]
    int weekDay;  // [0, 6] sunday = 0
    int yearDay;  // [0, 365]
    int tzOffset; // offset in minutes
} poww_time;

#define IO_RTC_INDEX 0x70
#define IO_RTC_TARGET 0x71

#define REG_SEC 0x00
#define REG_SEC_ALARM 0x01
#define REG_MIN 0x02
#define REG_MIN_ALARM 0x03
#define REG_HOUR 0x04
#define REG_HOUR_ALARM 0x05
#define REG_WEEK_DAY 0x06
#define REG_DAY 0x07
#define REG_MONTH 0x08
#define REG_YEAR 0x09
#define REG_A 0x0a
#define REG_B 0x0b
#define REG_C 0x0c
#define REG_D 0x0d

#define REGA_UIP (1 << 7)
#define REGB_HOURFORM (1 << 1)
#define REGB_DM (1 << 2)

static uint8_t read_rtc(uint8_t addr);
static void write_rtc(uint8_t addr, uint8_t val);
static uint8_t bcdtbin(uint8_t val);
static uint8_t bintbcd(uint8_t val);

extern volatile uint32_t g_pitTicks;
void pit_wait(unsigned int ms);

void get_time(poww_time *dt);

extern int g_localTimeZone;

void set_time(const poww_time *dt);

#endif