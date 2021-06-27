#include <stdint.h>

#include "time.h"

#include "clock.h"
#include "clockio.h"
#include "connection.h"
#include "delay.h"
#include "display.h"
#include "temperature_sensor.h"

#define TEMPERATURE_INTERVAL_US 10000000
#define SLEEP_DURATION_US 3600000000

#define UPTIME (*(volatile uint32_t *)0x3ff20c00)
#define ROLLING_TIMESTAMP {rolling_timestamp = UPTIME;}
#define ELAPSED (UPTIME - rolling_timestamp)

static volatile enum ClockState { CLOCK_INITIALIZING, CLOCK_READY, CLOCK_SENSING } state = CLOCK_INITIALIZING;

static volatile uint32_t rolling_timestamp = 0;

void clock_heartbeat()
{
    time_t timex;
    struct tm tmx;
 
    char output[32];

    struct TemperatureSensorReadings tsr;
    int32_t t;
    int32_t h;

    uint64_t sleep = SLEEP_DURATION_US;

    switch (state)
    {
	case CLOCK_INITIALIZING:
	    clockio_printf("Clock initializing...\n");
	    if (connection_ready() && display_ready() && temperature_sensor_ready()) {
		clockio_printf("Clock initialized in %d us\n", ELAPSED);
		state = CLOCK_SENSING;
	    } else {
		connection_init();
		display_init();
		temperature_sensor_init();
	    }
	    break;

	case CLOCK_READY:
	    timex = time(NULL);
	    localtime_r(&timex, &tmx);

	    clockio_printf("%02u:%02u:%02u Clock ready... Timestamp set %d us ago\n", tmx.tm_hour, tmx.tm_min, tmx.tm_sec, ELAPSED);
	    clockio_sprintf(output, "%02u:%02u:%02u   %02u/%02u", tmx.tm_hour, tmx.tm_min, tmx.tm_sec, tmx.tm_mday, tmx.tm_mon+1);

	    display_add(output);

	    if (temperature_sensor_ready()) {
		tsr = temperature_sensor_readings();
	    } else if (temperature_sensor_error()) {
		clockio_printf("Temperature sensor error\n");
		temperature_sensor_init();
	    }

	    t = tsr.temperature * 1000;
	    h = tsr.humidity * 1000;
	    clockio_printf("Temperature: %d.%d, Humidity: %d.%d\n", t / 1000, t % 1000, h / 1000, h % 1000);
	    clockio_sprintf(output, "T %d%cC    RH %d%%", t / 1000, 0xDF, h / 1000);

	    display_add(output);
	    display_refresh();

	    if (ELAPSED > TEMPERATURE_INTERVAL_US) {
		ROLLING_TIMESTAMP
		display_power(0);
		state = CLOCK_INITIALIZING;
		system_deep_sleep(sleep);
	    }
	    break;

	case CLOCK_SENSING:
	    temperature_sensor_measure();
	    state = CLOCK_READY;
	    ROLLING_TIMESTAMP
	    break;
    }
}
