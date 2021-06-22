#ifndef __DEBUG_H_
#define __DEBUG_H_

#include "osapi.h"

#define DEBUG(fmt, ...) os_printf(fmt, ## __VA_ARGS__)

#endif

