#include "osapi.h"

#include "delay.h"

#define MAX_DELAY 65535

void delay_us(uint32_t us)
{
    uint32_t wholes;
    uint32_t remainder;
    uint32_t counter;

    if (us <= MAX_DELAY) {
	os_delay_us(us);
	return;
    }

    wholes = us / MAX_DELAY;
    remainder = us % MAX_DELAY;

    for (counter = 0; counter < wholes; ++counter)
	os_delay_us(MAX_DELAY);

    os_delay_us(remainder);
}


