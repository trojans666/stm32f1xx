#include "hardware_entry.h"

void hardware_entry(void)
{
    hardware_systick_init(1,1);//1ms 1 = true;

    //��usart_printf�г�ͻ���Ǳ߿������жϡ����͵�ֵ��д��������ȥ
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
