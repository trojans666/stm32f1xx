#include "stm32f4xx_iwdg.h"
#include "hardware_watchdog.h"

//独立看门狗
//喂狗
void keep_alive()
{
    IWDG_ReloadCounter();
}

void wdt_set_timeout(unsigned short second)
{
    //Tout=((4*2^prer)*rlr)/32 (ms).
    IWDG_SetReload(second * 500);
}
unsigned int wdt_get_timeout()
{
	return 0;
}
unsigned int wdt_get_remain()
{
	return 0;
}

void wdt_enable()
{
    IWDG_Enable();
}

void hardware_watchdog_init(void)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(4);//预分频
}
