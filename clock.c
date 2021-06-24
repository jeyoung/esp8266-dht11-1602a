#include <stdint.h>

#include "time.h"

#include "clock.h"
#include "clockio.h"
#include "connection.h"
#include "delay.h"
#include "display.h"

#define UPTIME (*(volatile uint32_t *)0x3ff20c00)
#define ROLLING_TIMESTAMP {rolling_timestamp = UPTIME;}
#define ELAPSED (UPTIME - rolling_timestamp)

typedef enum { CLOCK_INITIALIZING, CLOCK_READY } ClockState;

static volatile ClockState state = CLOCK_INITIALIZING;
static volatile uint32_t rolling_timestamp;

void clock_heartbeat()
{
    time_t timex;
    struct tm tmx;
    
    char output[32];

    switch (state)
    {
	case CLOCK_INITIALIZING:
	    clockio_printf("Clock initializing...\n");
	    if (connection_ready() && display_ready()) {
		clockio_printf("Clock initialized in %d us\n", ELAPSED);
		state = CLOCK_READY;
	    } else {
		connection_init();
		display_init();
	    }
	    break;

	case CLOCK_READY:
	    timex = time(NULL);
	    localtime_r(&timex, &tmx);

	    clockio_printf("%d:%d:%d Clock ready... Timestamp set %d us ago\n", tmx.tm_hour, tmx.tm_min, tmx.tm_sec, ELAPSED);
	    clockio_sprintf(output, "%02u:%02u:%02u   %02u/%02u", tmx.tm_hour, tmx.tm_min, tmx.tm_sec, tmx.tm_mday, tmx.tm_mon+1);

	    display_add("TIME        DATE");
	    display_add(output);
	    display_refresh();

	    ROLLING_TIMESTAMP
	    break;
    }
}
