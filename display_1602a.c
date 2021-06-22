#include "ets_sys.h"
#include "gpio.h"

#include "debug.h"
#include "delay.h"

#define E  0
#define RS 15
#define D4 2
#define D5 14
#define D6 12
#define D7 13

static void write_byte(char byte)
{
    GPIO_OUTPUT_SET(D7, (byte & 0x80) >> 7);
    GPIO_OUTPUT_SET(D6, (byte & 0x40) >> 6);
    GPIO_OUTPUT_SET(D5, (byte & 0x20) >> 5);
    GPIO_OUTPUT_SET(D4, (byte & 0x10) >> 4);
    GPIO_OUTPUT_SET(E, 1);
    delay_us(5);
    GPIO_OUTPUT_SET(E, 0);

    GPIO_OUTPUT_SET(D7, (byte & 0x08) >> 3);
    GPIO_OUTPUT_SET(D6, (byte & 0x04) >> 2);
    GPIO_OUTPUT_SET(D5, (byte & 0x02) >> 1);
    GPIO_OUTPUT_SET(D4, (byte & 0x01) >> 0);
    GPIO_OUTPUT_SET(E, 1);
    delay_us(5);
    GPIO_OUTPUT_SET(E, 0);
}

static void write_instruction(uint8_t byte)
{
    GPIO_OUTPUT_SET(RS, 0);
    write_byte(byte);
}

static void write_data(uint8_t byte)
{
    GPIO_OUTPUT_SET(RS, 1);
    write_byte(byte);
}

static void write_string(const char * s)
{
    while (*s) {
	write_data(*s++);
	delay_us(40);
    }
}

void display_setup()
{
    DEBUG("Setting up display...\n");
    write_instruction(0x28);
    delay_us(40);
    write_instruction(0x0C);
    delay_us(40);
}

void display_clear()
{
    DEBUG("Clearing display...\n");
    write_instruction(0x01);
    delay_us(1600);
}

void display_write(uint8_t row, const char * s)
{
    uint8_t locations[2] = { 0x00, 0x40 };
    write_instruction(0x80 | locations[row]);
    delay_us(40);

    write_string(s);
}
