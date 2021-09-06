#include <stdint.h>
#include "ets_sys.h"
#include "user_interface.h"
#include "time.h"
#include "clock.h"
#include "clockio.h"
#include "connection.h"
#include "delay.h"
#include "display.h"
#include "temperature_sensor.h"

// 3 minutes
#define TEMPERATURE_INTERVAL_US 180000000

// 60 seconds
#define INITIALIZATION_TIMEOUT_US 60000000

// 120 seconds
#define PAIRING_TIMEOUT_US 120000000

// 1 second sleep
#define SLEEP_DURATION_US 1000000ul

#define UPTIME (*(volatile uint32_t *)0x3ff20c00)
#define ROLLING_TIMESTAMP {rolling_timestamp = UPTIME;}
#define ELAPSED (UPTIME - rolling_timestamp)
#define OFFSETS_COUNT 37

static float offsets[OFFSETS_COUNT] = { 14, 13, 12.75, 12, 11, 10.5, 10, 9.5, 9, 8.75, 8, 7, 6.5, 5.75, 5.5, 5, 4.5, 4, 3, 2, 1, 0, -1, -2, -2.5, -3, -4, -5, -6, -7, -8, -9, -9.5, -10, -11, -12 };

static volatile enum ClockState { CLOCK_PAIRING, CLOCK_INITIALIZING, CLOCK_READY, CLOCK_SLEEP, CLOCK_SENSING } state = CLOCK_INITIALIZING;
static volatile uint32_t rolling_timestamp = 0;
static volatile int8_t offset = 21;

void ICACHE_FLASH_ATTR clock_next()
{
    if (--offset < 0)
	offset = OFFSETS_COUNT - 1;
    state = CLOCK_READY;
}

void ICACHE_FLASH_ATTR clock_pair()
{
    if (state == CLOCK_PAIRING)
	return;
    state = CLOCK_PAIRING;
    ROLLING_TIMESTAMP
}

void ICACHE_FLASH_ATTR clock_heartbeat()
{
    uint64_t sleep = SLEEP_DURATION_US;
    time_t timex;
    time_t timex_adjusted;
    struct tm tmx;
    struct TemperatureSensorReadings tsr;
    int32_t t;
    int32_t h;
    char output[32];

    switch (state) {
    case CLOCK_PAIRING:
	if (ELAPSED > PAIRING_TIMEOUT_US) {
	    display_add("Not connected   ");
	    display_refresh();
	    state = CLOCK_SLEEP;
	} else if (connection_paired()) {
	    display_add("Connected       ");
	    display_refresh();
	    ROLLING_TIMESTAMP
	    state = CLOCK_INITIALIZING;
	} else {
	    display_add("Connecting...   ");
	    display_refresh();
	}
	break;
    case CLOCK_INITIALIZING:
	if (connection_ready() && display_ready() && temperature_sensor_ready()) {
	    state = CLOCK_SENSING;
	} else if (ELAPSED > INITIALIZATION_TIMEOUT_US) {
	    state = CLOCK_SLEEP;
	} else {
	    connection_init();
	    display_init();
	    temperature_sensor_init();
	}
	break;
    case CLOCK_SLEEP:
	ROLLING_TIMESTAMP
	display_power(0);
	state = CLOCK_INITIALIZING;
	system_deep_sleep(sleep);
	break;
    case CLOCK_READY:
	timex = time(NULL);
	timex_adjusted = timex + (time_t)(offsets[offset] * 60 * 60);
	gmtime_r(&timex_adjusted, &tmx);

	clockio_sprintf(output, "%02u:%02u:%02u   %02u/%02u", tmx.tm_hour, tmx.tm_min, tmx.tm_sec, tmx.tm_mday, tmx.tm_mon+1);
	display_add(output);
	if (temperature_sensor_ready()) {
	    tsr = temperature_sensor_readings();
	    t = tsr.temperature * 1000;
	    h = tsr.humidity * 1000;
	    clockio_sprintf(output, "T %d%cC    RH %d%%", t / 1000, 0xDF, h / 1000);
	    display_add(output);
	} else if (temperature_sensor_error()) {
	    temperature_sensor_init();
	}
	display_refresh();
	if (ELAPSED > TEMPERATURE_INTERVAL_US)
	    state = CLOCK_SENSING;
	break;
    case CLOCK_SENSING:
	temperature_sensor_measure();
	state = CLOCK_READY;
	ROLLING_TIMESTAMP
	break;
    }
}
