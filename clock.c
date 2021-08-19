#include <stdint.h>
#include "time.h"
#include "clock.h"
#include "clockio.h"
#include "connection.h"
#include "delay.h"
#include "display.h"
#include "temperature_sensor.h"

// 3 minutes
#define TEMPERATURE_INTERVAL_US 180000000

// 15 seconds
#define INITIALIZATION_TIMEOUT 15000000

// 1 second sleep
#define SLEEP_DURATION_US 1000000ul

#define UPTIME (*(volatile uint32_t *)0x3ff20c00)
#define ROLLING_TIMESTAMP {rolling_timestamp = UPTIME;}
#define ELAPSED (UPTIME - rolling_timestamp)

static volatile enum ClockState { CLOCK_INITIALIZING, CLOCK_READY, CLOCK_SLEEP, CLOCK_SENSING } state = CLOCK_INITIALIZING;
static volatile uint32_t rolling_timestamp = 0;

void clock_heartbeat()
{
    uint64_t sleep = SLEEP_DURATION_US;
    time_t timex;
    struct tm tmx;
    struct TemperatureSensorReadings tsr;
    int32_t t;
    int32_t h;
    char output[32];

    switch (state) {
    case CLOCK_INITIALIZING:
#if 0
	clockio_printf("Clock initializing...\n");
#endif
	if (ELAPSED > INITIALIZATION_TIMEOUT)
	    state = CLOCK_SLEEP;
	else if (connection_ready() && display_ready() && temperature_sensor_ready()) {
#if 0
	    clockio_printf("Clock initialized in %d us\n", ELAPSED);
#endif
	    state = CLOCK_SENSING;
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
	localtime_r(&timex, &tmx);
#if 0
	clockio_printf("%02u:%02u:%02u Clock ready... Timestamp set %d us ago\n", tmx.tm_hour, tmx.tm_min, tmx.tm_sec, ELAPSED);
	clockio_sprintf(output, "%02u:%02u:%02u   %02u/%02u", tmx.tm_hour, tmx.tm_min, tmx.tm_sec, tmx.tm_mday, tmx.tm_mon+1);
#endif
	clockio_sprintf(output, "%02u:%02u      %02u/%02u", tmx.tm_hour, tmx.tm_min, tmx.tm_mday, tmx.tm_mon+1);
	display_add(output);
	if (temperature_sensor_ready()) {
	    tsr = temperature_sensor_readings();
	    t = tsr.temperature * 1000;
	    h = tsr.humidity * 1000;
#if 0
	    clockio_printf("Temperature: %d.%d, Humidity: %d.%d\n", t / 1000, t % 1000, h / 1000, h % 1000);
#endif
	    clockio_sprintf(output, "T %d%cC    RH %d%%", t / 1000, 0xDF, h / 1000);
	    display_add(output);
	} else if (temperature_sensor_error()) {
#if 0
	    clockio_printf("Temperature sensor error\n");
#endif
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
