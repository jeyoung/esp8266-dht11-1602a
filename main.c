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

// user_pre_init is required from SDK v3.0.0 onwards
// It is used to register the parition map with the SDK, primarily to allow
// the app to use the SDK's OTA capability.  We don't make use of that in
// otb-iot and therefore the only info we provide is the mandatory stuff:
// - RF calibration data
// - Physical data
// - System parameter
// The location and length of these are from the 2A SDK getting started guide
void ICACHE_FLASH_ATTR user_pre_init(void)
{
  bool rc = false;
  static const partition_item_t part_table[] =
  {
    {SYSTEM_PARTITION_RF_CAL,
     0x3fb000,
     0x1000},
    {SYSTEM_PARTITION_PHY_DATA,
     0x3fc000,
     0x1000},
    {SYSTEM_PARTITION_SYSTEM_PARAMETER,
     0x3fd000,
     0x3000},
  };

  // This isn't an ideal approach but there's not much point moving on unless
  // or until this has succeeded cos otherwise the SDK will just barf and
  // refuse to call user_init()
  while (!rc)
  {
    rc = system_partition_table_regist(part_table,
				       sizeof(part_table)/sizeof(part_table[0]),
                                       4);
  }

  return;
}
