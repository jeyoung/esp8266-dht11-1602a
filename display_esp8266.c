#include "ets_sys.h"
#include "gpio.h"
#include "clockio.h"
#include "delay.h"

#define GPIO_E  0
#define GPIO_RS 15
#define GPIO_D4 2
#define GPIO_D5 14
#define GPIO_D6 12
#define GPIO_D7 13
#define GPIO_POWER 4

static void write_byte(char byte)
{
    GPIO_OUTPUT_SET(GPIO_D7, (byte & 0x80) >> 7);
    GPIO_OUTPUT_SET(GPIO_D6, (byte & 0x40) >> 6);
    GPIO_OUTPUT_SET(GPIO_D5, (byte & 0x20) >> 5);
    GPIO_OUTPUT_SET(GPIO_D4, (byte & 0x10) >> 4);
    GPIO_OUTPUT_SET(GPIO_E, 1);
    delay_us(5);
    GPIO_OUTPUT_SET(GPIO_E, 0);

    GPIO_OUTPUT_SET(GPIO_D7, (byte & 0x08) >> 3);
    GPIO_OUTPUT_SET(GPIO_D6, (byte & 0x04) >> 2);
    GPIO_OUTPUT_SET(GPIO_D5, (byte & 0x02) >> 1);
    GPIO_OUTPUT_SET(GPIO_D4, (byte & 0x01) >> 0);
    GPIO_OUTPUT_SET(GPIO_E, 1);
    delay_us(5);
    GPIO_OUTPUT_SET(GPIO_E, 0);
}

static void write_instruction(uint8_t byte)
{
    GPIO_OUTPUT_SET(GPIO_RS, 0);
    write_byte(byte);
}

static void write_data(uint8_t byte)
{
    GPIO_OUTPUT_SET(GPIO_RS, 1);
    write_byte(byte);
}

static void write_string(const char *s)
{
    while (*s) {
	write_data(*s++);
	delay_us(40);
    }
}

void display_power(uint8_t onoff)
{
    GPIO_OUTPUT_SET(GPIO_POWER, onoff);
}

void display_setup()
{
    clockio_printf("Setting up display...\n");
    write_instruction(0x28);
    delay_us(40);
    write_instruction(0x0C);
    delay_us(40);
}

void display_clear()
{
    clockio_printf("Clearing display...\n");
    write_instruction(0x01);
    delay_us(1600);
}

void display_write(uint8_t row, const char *s)
{
    uint8_t locations[2] = { 0x00, 0x40 };

    write_instruction(0x80 | locations[row]);
    delay_us(40);
    write_string(s);
}
