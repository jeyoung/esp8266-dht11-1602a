#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "delay.h"

#define MAX_DELAY 65535

extern void * ets_vsprintf(void *, void *, void *);

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
    for (counter = 0; counter < wholes; ++counter) {
	os_delay_us(MAX_DELAY);
	system_soft_wdt_feed();
    }
    os_delay_us(remainder);
}


