#include "ets_sys.h"
#include "gpio.h"
#include "hw_timer.c"
#include "osapi.h"
#include "uart.h"

#include "clock.h"
#include "delay.h"

#define HW_TIMER_INTERVAL 250000

void ICACHE_FLASH_ATTR hw_timer_func(void)
{
    clock_heartbeat();
    system_soft_wdt_feed();
    hw_timer_arm(HW_TIMER_INTERVAL);
}

void ICACHE_FLASH_ATTR user_init(void)
{
    uart_init(BIT_RATE_921600, BIT_RATE_921600);

    gpio_init();

    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);
    PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO5_U);

    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_GPIO15);

    hw_timer_init(FRC1_SOURCE, 0);
    hw_timer_set_func(hw_timer_func);
    hw_timer_arm(HW_TIMER_INTERVAL);
} 
