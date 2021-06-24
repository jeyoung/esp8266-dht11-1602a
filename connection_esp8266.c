#include "osapi.h"
#include "user_interface.h"

#include "clockio.h"
#include "connection.h"

typedef enum { CONNECTION_INITIALIZING, CONNECTION_SNTP, CONNECTION_READY } ConnectionState;

static volatile ConnectionState state = CONNECTION_INITIALIZING;

uint8_t connection_ready()
{
    return state == CONNECTION_READY;
}

uint32_t connection_sntp_get_timestamp()
{
    return sntp_get_current_timestamp();
}

void connection_init()
{
    switch (state) {
	case CONNECTION_INITIALIZING:
	    clockio_printf("WiFi connecting...\n");
	    if (wifi_station_get_connect_status() == STATION_GOT_IP) {
		clockio_printf("WiFi connected\n");
		sntp_setservername(0, "0.pool.ntp.org");
		sntp_setservername(1, "1.pool.ntp.org");
		sntp_setservername(2, "2.pool.ntp.org");
		sntp_setservername(3, "ntp.sjtu.edu.cn");
		sntp_stop();
		if (sntp_set_timezone(0)) {
		    sntp_init();
		}
		state = CONNECTION_SNTP;
	    }
	    break;

	case CONNECTION_SNTP:
	    clockio_printf("SNTP initializing...\n");
	    if (sntp_get_current_timestamp() != 0) {
		clockio_printf("SNTP initialized\n");
		state = CONNECTION_READY;
	    }
	    break;

	case CONNECTION_READY:
	    clockio_printf("Connection ready\n");
	    break;
    }
}


