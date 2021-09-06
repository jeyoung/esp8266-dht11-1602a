#include <stdarg.h>
#include "osapi.h"
#include "clockio.h"

extern int ets_vsprintf(const char *, ...);

int clockio_printf(const char *fmt, ...)
{
    int ret;
    char str[1024];
    va_list ap;

    va_start(ap, fmt);
    ret = ets_vsprintf(str, fmt, ap);
    os_printf(str);
    va_end(ap);
    return ret;
}

int clockio_sprintf(char *str, const char *fmt, ...)
{
    int ret;
    va_list ap;

    va_start(ap, fmt);
    ret = ets_vsprintf(str, fmt, ap);
    va_end(ap);
    return ret;
}
