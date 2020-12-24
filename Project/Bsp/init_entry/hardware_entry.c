#include "hardware_entry.h"

void hardware_entry(void)
{
    hardware_systick_init(1,1);//1ms 1 = true;

    //和usart_printf有冲突。那边开启了中断。发送的值会写到队列里去
    //hardware_usart_init();
    //systick_register_callback("usart_checkover",hardware_checkover);

    hardware_usart_printf_init();

    hardware_rtc_init();

    hardware_timer_init();

    hardware_spi_init();

    //hardware_i2c_init();

    //hardware_watchdog_init();
	
		//hardware_pwm_init();
	
		hardware_flash_init();
}
