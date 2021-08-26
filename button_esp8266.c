#include <stdint.h>
#include "osapi.h"
#include "user_interface.h"
#include "button.h"

#define BUTTON_PIN 0

static volatile uint16_t button_bounces = 0xFFFF;
static volatile uint16_t button_ignore = 0;
static volatile uint32_t button_pressed_timestamp = 0;
static volatile uint16_t button_down = 0;
static volatile uint16_t button_up = 0;

void ICACHE_FLASH_ATTR button_read()
{
    GPIO_DIS_OUTPUT(BUTTON_PIN);
    button_bounces = (button_bounces << 1) | (uint16_t)GPIO_INPUT_GET(BUTTON_PIN);
    button_down = !button_ignore && (button_bounces < 0xFF00);
    button_up = !button_ignore && button_down && (button_bounces & 0x00FF);

    if (button_ignore && system_get_time() - button_pressed_timestamp > 2000000) {
	button_bounces = 0xFFFF;
	button_ignore = 0;
	button_pressed_timestamp = 0;
    }

    if (!button_down)
	button_pressed_timestamp = 0;
    else if (button_pressed_timestamp == 0)
	button_pressed_timestamp = system_get_time();

}

uint16_t ICACHE_FLASH_ATTR button_wps()
{
    uint8_t result = 0;

    if (button_down && system_get_time() - button_pressed_timestamp > 3000000) {
	button_bounces = 0xFFFF;
	button_ignore = 1;
	button_pressed_timestamp = 0;
	result = 1;
    }

    return result;
}

uint16_t ICACHE_FLASH_ATTR button_next()
{
    if (button_up) {
	button_bounces = 0xFFFF;
	button_pressed_timestamp = 0;
    }
    return button_up;
}
