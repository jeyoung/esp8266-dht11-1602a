#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <stdint.h>

uint8_t display_ready();

void display_init();

void display_power(uint8_t onoff);

void display_add(const char *s);

void display_refresh();

#endif
