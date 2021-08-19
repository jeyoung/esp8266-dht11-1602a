#include "osapi.h"
#include "user_interface.h"
#include "clockio.h"
#include "connection.h"

static volatile enum ConnectionState { CONNECTION_INITIALIZING, CONNECTION_SNTP, CONNECTION_READY } state = CONNECTION_INITIALIZING;

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
#if 0
	clockio_printf("WiFi connecting...\n");
#endif
	if (wifi_station_get_connect_status() == STATION_GOT_IP) {
#if 0
	    clockio_printf("WiFi connected\n");
#endif
	    sntp_setservername(0, "0.pool.ntp.org");
	    sntp_setservername(1, "1.pool.ntp.org");
	    sntp_setservername(2, "2.pool.ntp.org");
	    sntp_setservername(3, "ntp.sjtu.edu.cn");
	    sntp_stop();
	    if (sntp_set_timezone(1))
		sntp_init();
	    state = CONNECTION_SNTP;
	}
	break;
    case CONNECTION_SNTP:
#if 0
	clockio_printf("SNTP initializing...\n");
#endif
	if (sntp_get_current_timestamp() != 0) {
#if 0
	    clockio_printf("SNTP initialized\n");
#endif
	    state = CONNECTION_READY;
	}
	break;
    case CONNECTION_READY:
#if 0
	clockio_printf("Connection ready\n");
#endif
	break;
    }
}


