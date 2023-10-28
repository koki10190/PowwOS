#include <cpu/io.h>
#include <uart.h>
#include <time/rtc.h>

static inline void IoWrite8(unsigned int port, uint8_t data) {
    __asm__ volatile("outb %b0, %w1"
                     :
                     : "a"(data), "Nd"(port));
}

static uint8_t read_rtc(uint8_t addr) {
    IoWrite8(IO_RTC_INDEX, addr);
    return IoRead8(IO_RTC_TARGET);
}

static void write_rtc(uint8_t addr, uint8_t val) {
    IoWrite8(IO_RTC_INDEX, addr);
    IoWrite8(IO_RTC_TARGET, val);
}

static uint8_t bcdtbin(uint8_t val) {
    return (val & 0xf) + (val >> 4) * 10;
}

static uint8_t bintbcd(uint8_t val) {
    return ((val / 10) << 4) + (val % 10);
}

volatile uint32_t g_pitTicks;
void pit_wait(unsigned int ms) {
    uint32_t now = g_pitTicks;
    ++ms;

    while (g_pitTicks - now < ms) {
        ;
    }
}

void get_time(poww_time *dt) {
    uint8_t sec = read_rtc(REG_SEC);
    uint8_t min = read_rtc(REG_MIN);
    uint8_t hour = read_rtc(REG_HOUR);
    uint8_t weekDay = read_rtc(REG_WEEK_DAY);
    uint8_t day = read_rtc(REG_DAY);
    uint8_t month = read_rtc(REG_MONTH);
    uint16_t year = read_rtc(REG_YEAR);

    uint8_t regb = read_rtc(REG_B);

    if (~regb & REGB_DM) {
        sec = bcdtbin(sec);
        min = bcdtbin(min);
        hour = bcdtbin(hour);
        day = bcdtbin(day);
        month = bcdtbin(month);
        year = bcdtbin(year);
    }

    year += 2000;
    weekDay--;

    dt->sec = sec;
    dt->min = min;
    dt->hour = hour;
    dt->day = day;
    dt->month = month;
    dt->year = year;
    dt->weekDay = weekDay;
    dt->tzOffset = g_localTimeZone;
}

int g_localTimeZone = -7 * 60;

void set_time(const poww_time *dt) {
    uint8_t sec = dt->sec;
    uint8_t min = dt->min;
    uint8_t hour = dt->hour;
    uint8_t day = dt->day;
    uint8_t month = dt->month;
    uint8_t year = dt->year - 2000;
    uint8_t weekDay = dt->weekDay + 1;

    if (sec >= 60 || min >= 60 || hour >= 24 || day > 31 || month > 12 || year >= 100 || weekDay > 7) {
        uart_puts("set_time: bad data\n");
        return;
    }

    uint8_t regb = read_rtc(REG_B);

    if (~regb & REGB_DM) {
        sec = bintbcd(sec);
        min = bintbcd(min);
        hour = bintbcd(hour);
        day = bintbcd(day);
        month = bintbcd(month);
        year = bintbcd(year);
    }

    if (read_rtc(REG_A) & REGA_UIP) {
        pit_wait(3);
    }

    write_rtc(REG_SEC, sec);
    write_rtc(REG_MIN, min);
    write_rtc(REG_HOUR, hour);
    write_rtc(REG_WEEK_DAY, weekDay);
    write_rtc(REG_DAY, day);
    write_rtc(REG_MONTH, month);
    write_rtc(REG_YEAR, year);
}