#include <stdint.h>

#include "time.h"

#include "clock.h"
#include "connection.h"
#include "debug.h"
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
	    DEBUG("Clock initializing...\n");
	    if (connection_ready() && display_ready()) {
		DEBUG("Clock initialized in %d us\n", ELAPSED);
		state = CLOCK_READY;
	    } else {
		connection_init();
		display_init();
	    }
	    break;

	case CLOCK_READY:
	    timex = time(NULL);
	    localtime_r(&timex, &tmx);

	    DEBUG("%02d:%02d:%02d Clock ready... Timestamp set %d us ago\n", tmx.tm_hour, tmx.tm_min, tmx.tm_sec, ELAPSED);
	    os_sprintf(output, "%02d:%02d:%02d   %02d/%02d", tmx.tm_hour, tmx.tm_min, tmx.tm_sec, tmx.tm_mday, tmx.tm_mon+1);

	    display_add("TIME        DATE");
	    display_add(output);
	    display_refresh();

	    ROLLING_TIMESTAMP
	    break;
    }
}
