#include "temperature_sensor.h"
#include "ets_sys.h"
#include "gpio.h"
#include "clockio.h"
#include "delay.h"

#define TEMPERATURE_SENSOR_WAKEUP_TIME_US 3000000
#define READ_COUNT_MAX 3

#define GPIO_WIRE 5

static volatile enum WireState { WIRE_INITIALIZING, WIRE_READY, WIRE_STARTED, WIRE_WAITING, WIRE_RESPONDED, WIRE_NO_RESPONSE, WIRE_DATA_PREPARATION, WIRE_DATA_STARTING, WIRE_DATA_ZERO, WIRE_DATA_ONE, WIRE_DATA_RECEIVED, WIRE_DONE } state = WIRE_INITIALIZING;

static uint8_t rh_integral;
static uint8_t rh_decimal;
static uint8_t t_integral;
static uint8_t t_decimal;

uint8_t temperature_sensor_read()
{
    enum WireState previous_state;
    uint32_t state_timestamp;
    uint8_t read_count = 0;
    uint32_t elapsed;
    uint32_t data = 0;
    uint8_t checksum = 0;
    uint8_t bit_count = 0;

#if 0
    clockio_printf("Temperature sensor reading...\n");
#endif
    state = WIRE_INITIALIZING;
    state_timestamp = system_get_time();
start_read:
#if 0
    clockio_printf("Read count #%d\n", read_count+1);
#endif
    while (state != WIRE_DONE && state != WIRE_NO_RESPONSE) {
	previous_state = state;
	switch (state) {
	case WIRE_INITIALIZING:
	    state_timestamp = system_get_time();
	    GPIO_OUTPUT_SET(GPIO_WIRE, 1);
	    state = WIRE_READY;
	    break;
	case WIRE_READY:
	    GPIO_OUTPUT_SET(GPIO_WIRE, 0);
	    state_timestamp = system_get_time();
	    state = WIRE_STARTED;
	    break;
	case WIRE_STARTED:
	    if (elapsed >= 18000) {
		state_timestamp = system_get_time();
		GPIO_OUTPUT_SET(GPIO_WIRE, 1);
		GPIO_DIS_OUTPUT(GPIO_WIRE);
		state = WIRE_WAITING;
	    }
	    break;
	case WIRE_WAITING:
	    if (!GPIO_INPUT_GET(GPIO_WIRE)) {
		state_timestamp = system_get_time();
		state = WIRE_RESPONDED;
	    } else if (elapsed > 40)
		state = WIRE_NO_RESPONSE;
	    break;
	case WIRE_NO_RESPONSE:
	    break;
	case WIRE_RESPONDED:
	    if (GPIO_INPUT_GET(GPIO_WIRE)) {
		state_timestamp = system_get_time();
		state = WIRE_DATA_PREPARATION;
	    } else if (elapsed > 85)
		state = WIRE_NO_RESPONSE;
	    break;
	case WIRE_DATA_PREPARATION:
	    if (!GPIO_INPUT_GET(GPIO_WIRE)) {
		state_timestamp = system_get_time();
		state = WIRE_DATA_STARTING;
	    } else if (elapsed > 85)
		state = WIRE_NO_RESPONSE;
	    break;
	case WIRE_DATA_STARTING:
	    if (GPIO_INPUT_GET(GPIO_WIRE)) {
		state_timestamp = system_get_time();
		state = WIRE_DATA_ZERO;
	    } else if (elapsed > 55)
		state = WIRE_NO_RESPONSE;
	    break;
	case WIRE_DATA_ZERO:
	    if (!GPIO_INPUT_GET(GPIO_WIRE)) {
		state_timestamp = system_get_time();
		if (bit_count < 32)
		    data <<= 1;
		else
		    checksum <<= 1;
		if (++bit_count < 40)
		    state = WIRE_DATA_STARTING;
		else
		    state = WIRE_DATA_RECEIVED;
	    } else if (elapsed > 28)
		state = WIRE_DATA_ONE;
	    break;
	case WIRE_DATA_ONE:
	    if (!GPIO_INPUT_GET(GPIO_WIRE)) {
		state_timestamp = system_get_time();
		if (bit_count < 32) {
		    data <<= 1;
		    data |= 0x00000001;
		} else {
		    checksum <<= 1;
		    checksum |= 0x01;
		}
		if (++bit_count < 40)
		    state = WIRE_DATA_STARTING;
		else
		    state = WIRE_DATA_RECEIVED;
	    } else if (elapsed > 70)
		state = WIRE_NO_RESPONSE;
	    break;
	case WIRE_DATA_RECEIVED:
	    if (GPIO_INPUT_GET(GPIO_WIRE)) {
		state_timestamp = system_get_time();
		GPIO_OUTPUT_SET(GPIO_WIRE, 1);
		state = WIRE_DONE;
	    } else if (elapsed > 55)
		state = WIRE_NO_RESPONSE;
	    break;
	}
	elapsed = system_get_time() - state_timestamp;
    }
#if 0
    clockio_printf("Data counter: %d, Elapsed: %u, Previous state: %u, State: %u\n", bit_count, elapsed, previous_state, state);
#endif
    rh_integral = (data & 0xFF000000) >> 24;
    rh_decimal = (data & 0x00FF0000) >> 16;
    t_integral = (data & 0x0000FF00) >> 8;
    t_decimal = (data & 0x000000FF);

#if 1
    clockio_printf("data: %u, rh_integral: %u, rh_decimal: %u, t_integral: %u, t_decimal: %u, checksum: %u\n", data, rh_integral, rh_decimal, t_integral, t_decimal, checksum);
#endif
    if (checksum > 0 && rh_integral + rh_decimal + t_integral + t_decimal != checksum && read_count++ < READ_COUNT_MAX) {
	state = WIRE_INITIALIZING;
	goto start_read;
    }
    return state != WIRE_NO_RESPONSE;
}

float temperature_sensor_temperature()
{
    return t_integral + (t_decimal / 1000);
}

float temperature_sensor_humidity()
{
    return rh_integral + (rh_decimal / 1000);
}

void temperature_sensor_start()
{
    clockio_printf("Temperature sensor starting...\n");
    delay_us(TEMPERATURE_SENSOR_WAKEUP_TIME_US);
    state = WIRE_INITIALIZING;
}



