#ifndef _TEMPERATURE_SENSOR_H_
#define _TEMPERATURE_SENSOR_H_

#include <stdint.h>

struct TemperatureSensorReadings {
    float humidity;
    float temperature;
};

void temperature_sensor_init();

uint8_t temperature_sensor_ready();

uint8_t temperature_sensor_error();

void  temperature_sensor_measure();

struct TemperatureSensorReadings temperature_sensor_readings();

#endif
