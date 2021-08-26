#include "osapi.h"
#include "user_interface.h"
#include "clockio.h"
#include "connection.h"

#define TIMEZONE_COUNT 25

static volatile enum ConnectionState { CONNECTION_PAIRING, CONNECTION_PAIRED, CONNECTION_PAIRING_FAILED, CONNECTION_INITIALIZING, CONNECTION_SNTP, CONNECTION_READY } state = CONNECTION_INITIALIZING;

void ICACHE_FLASH_ATTR connection_wps_cb(int status)
{
    switch (status) {
    case WPS_CB_ST_SUCCESS:
	wifi_wps_disable();
	wifi_station_connect();
	state = CONNECTION_PAIRED;
	break;
    default:
	wifi_wps_disable();
	state = CONNECTION_PAIRING_FAILED;
	break;
    }
}

uint8_t ICACHE_FLASH_ATTR connection_paired()
{
    uint8_t result = 0;
    switch (state) {
    case CONNECTION_PAIRING:
	break;
    case CONNECTION_PAIRED:
	state = CONNECTION_INITIALIZING;
	result = 1;
	break;
    default:
	wifi_wps_disable();
	wifi_wps_enable(WPS_TYPE_PBC);
	wifi_set_wps_cb(connection_wps_cb);
	wifi_wps_start();
	state = CONNECTION_PAIRING;
	break;
    }
    return result;
}

uint8_t connection_ready()
{
    return state == CONNECTION_READY;
}

uint32_t ICACHE_FLASH_ATTR connection_sntp_get_timestamp()
{
    return sntp_get_current_timestamp();
}

void ICACHE_FLASH_ATTR connection_init()
{
    switch (state) {
    case CONNECTION_INITIALIZING:
	if (wifi_station_get_connect_status() == STATION_GOT_IP) {
	    sntp_setservername(0, "0.pool.ntp.org");
	    sntp_setservername(1, "1.pool.ntp.org");
	    sntp_setservername(2, "2.pool.ntp.org");
	    sntp_setservername(3, "ntp.sjtu.edu.cn");
	    sntp_stop();
	    if (sntp_set_timezone(0))
		sntp_init();
	    state = CONNECTION_SNTP;
	}
	break;
    case CONNECTION_SNTP:
	if (sntp_get_current_timestamp() != 0) {
	    state = CONNECTION_READY;
	}
	break;
    case CONNECTION_READY:
	break;
    }
}
