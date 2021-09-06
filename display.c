#include <stdint.h>

#include "display.h"

#define MAX_LINES 2
#define MAX_COLUMNS 16

static volatile enum DisplayState { DISPLAY_INITIALIZING, DISPLAY_READY, DISPLAY_DATA_AVAILABLE, DISPLAY_WRITING } state = DISPLAY_INITIALIZING;

static char lines[MAX_LINES][MAX_COLUMNS+1];
static volatile uint32_t line_counter = 0;
static volatile uint32_t line_index = 0;

void display_power(uint8_t onoff);
void display_setup();
void display_clear();
void display_write(uint8_t row, const char *s);

uint8_t display_ready()
{
    return state == DISPLAY_READY;
}

void display_init()
{
    switch (state) {
    case DISPLAY_INITIALIZING:
	display_power(1);
	display_setup();
	display_clear();
	state = DISPLAY_READY;
	break;
    case DISPLAY_READY:
	for (line_index = 0; line_index < MAX_LINES; ++line_index)
	    lines[line_index][0] = '\0';
	line_index = 0;
	line_counter = 0;
	break;
    default:
	break;
    }
}

void display_add(const char *s)
{
    uint8_t column_counter = 0;

    while (*s && column_counter < MAX_COLUMNS)
	lines[line_counter % MAX_LINES][column_counter++] = *s++;
    lines[line_counter % MAX_LINES][column_counter] = '\0';
    ++line_counter;
    if (state == DISPLAY_READY)
	state = DISPLAY_DATA_AVAILABLE;
}

void display_refresh()
{
    switch (state) {
    case DISPLAY_DATA_AVAILABLE:
	if (line_counter > line_index)
	    state = DISPLAY_WRITING;
	break;
    case DISPLAY_WRITING:
	display_write(line_index % MAX_LINES, lines[line_index % MAX_LINES]);
	++line_index;
	state = DISPLAY_READY;
	break;
    case DISPLAY_READY:
    default:
	break;
    }
}

