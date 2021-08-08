#include "temperature_sensor.h"

static volatile enum TemperatureSensorState { TEMPERATURE_SENSOR_INTIALIZING, TEMPERATURE_SENSOR_READY, TEMPERATURE_SENSOR_ERROR } state = TEMPERATURE_SENSOR_INTIALIZING;

static struct TemperatureSensorReadings readings = {};

void temperature_sensor_start();

uint8_t temperature_sensor_read();

float temperature_sensor_temperature();

float temperature_sensor_humidity();

void temperature_sensor_init()
{
    switch (state) {
    case TEMPERATURE_SENSOR_INTIALIZING:
    case TEMPERATURE_SENSOR_ERROR:
	temperature_sensor_start();
	state = TEMPERATURE_SENSOR_READY;
	break;
    case TEMPERATURE_SENSOR_READY:
	break;
    }
}

uint8_t temperature_sensor_ready()
{
    return state == TEMPERATURE_SENSOR_READY;
}

uint8_t temperature_sensor_error()
{
    return state == TEMPERATURE_SENSOR_ERROR;
}

void temperature_sensor_measure()
{
    if (state == TEMPERATURE_SENSOR_READY) {
	if (temperature_sensor_read()) {
	    readings.temperature = temperature_sensor_temperature();
	    readings.humidity = temperature_sensor_humidity();
	} else
	    state = TEMPERATURE_SENSOR_ERROR;
    }
}

struct TemperatureSensorReadings temperature_sensor_readings()
{
    return readings;
}
