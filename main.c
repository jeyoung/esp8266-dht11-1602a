#include "ets_sys.h"
#include "gpio.h"
#include "osapi.h"
#include "uart.h"
#include "user_interface.h"
#include "button.h"
#include "clock.h"
#include "delay.h"

#define OS_TIMER_INTERVAL 125

static os_timer_t os_timer;

void os_timer_func(void *arg)
{
    button_read();
    uint8_t wps = button_wps();
    uint8_t next = button_next();
    if (next) {
	clock_next();
    } else if (wps) {
	clock_pair();
    }
    clock_heartbeat();
    os_timer_arm(&os_timer, OS_TIMER_INTERVAL, 0);
}

void ICACHE_FLASH_ATTR user_init(void)
{
    uart_init(BIT_RATE_921600, BIT_RATE_921600);
    gpio_init();
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0);
    PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO0_U);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
    PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO4_U);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);
    PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO5_U);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_GPIO15);
    PIN_PULLUP_DIS(PERIPHS_IO_MUX_MTDO_U);

#if 0
    wifi_set_sleep_type(LIGHT_SLEEP_T);
#endif

    os_timer_setfn(&os_timer, (os_timer_func_t *)os_timer_func, NULL);
    os_timer_arm(&os_timer, OS_TIMER_INTERVAL, 0);
} 
