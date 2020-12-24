#include "hardware_systick.h"

#include "stm32f10x_rcc.h"
#include <stdio.h>
#include <string.h>

void register_task(unsigned int timeout,void (*task)(void *args),void *args);
void unregister_task(void );
void set_reload_value(unsigned int ms);
unsigned int get_reload_value(void);
unsigned int get_cur_value(void);
void irq_enable(void);
void irq_disable(void);
unsigned int get_tick(void);

#ifdef NO_OS_USED
void delay_ms(unsigned int ms);
#else
void os_delay_ms(unsigned int ms);
#endif 

struct systick_dev s_systick = 
{
	72, //72m
	0,
	0,
	NULL,
	NULL,
	0,
	register_task,
	unregister_task,
	set_reload_value,
	get_reload_value,
	get_cur_value,
	irq_enable,
	irq_disable,
	get_tick,
#ifdef NO_OS_USED
	delay_ms
#else
	os_delay_ms
#endif 
};


void register_task(unsigned int timeout,void (*task)(void *args),void *args)
{
	if(timeout > 0 && task)
	{
		s_systick.task_timeout = timeout;
		s_systick.cur_task = task; 
		s_systick.task_args = args;
	}
}
	
void unregister_task()
{
	s_systick.cur_task = NULL;
	s_systick.task_timeout = 0;
}
	
void set_reload_value(unsigned int ms)
{
	 SysTick->CTRL &= ~(1UL << 0); //禁止

    unsigned int reload = s_systick.max_hz / 8; // 8分频
    reload *= 1000;  //1ms

    SysTick->LOAD = (reload * 1) - 1;

    s_systick.per_ms = SysTick->LOAD;

    SysTick->CTRL |= (1UL << 1); //1 bit 开启中断
    SysTick->CTRL |= (1UL << 0);
}
	
unsigned int get_reload_value(void)
{
	return SysTick->LOAD;
}
	
unsigned int get_cur_value(void)
{
	return SysTick->VAL;
}
	
void irq_enable(void)
{
	SysTick->CTRL |= (1UL << 1); //1 bit 开启中断
  SysTick->CTRL |= (1UL << 0);//SysTick_CTRL_ENABLE_Msk;//开始systick
}

void irq_disable(void)
{
	SysTick->CTRL &= ~(1UL << 0);
}
	
unsigned int get_tick(void)
{
	return s_systick.tick;
}

#ifdef NO_OS_USED
void delay_ms(unsigned int ms)
{
		unsigned int ticks_old = SysTick->VAL;
    unsigned int ticks_new = 0;
    unsigned int ticks_sum = 0;
    unsigned int ticks_delta = ms * s_systick.per_ms;

    if(ms > 25000)
        return ;

    while(1)
    {
        ticks_new = SysTick->VAL;
        if(ticks_new != ticks_old)
        {
            if(ticks_new < ticks_old)
            {
                ticks_sum += ticks_old - ticks_new;
            }
            else
            {
                ticks_sum += s_systick.per_ms - ticks_new + ticks_old;
            }

            ticks_old = ticks_new;

            if(ticks_sum >= ticks_delta)
                break;
        }
    }
}
#else
void os_delay_ms(unsigned int ms)
{

}
#endif 

void hardware_systick_init()
{
    unsigned int reload;
    //配置时钟源
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);

    reload = s_systick.max_hz / 8; // 8分频
    reload *= 1000;  //1ms

    SysTick->LOAD = (reload * 1) - 1; //重载数值

    s_systick.per_ms = SysTick->LOAD;

    if(1)
    {
        SysTick->CTRL |= (1UL << 1); //1 bit 开启中断
        SysTick->CTRL |= (1UL << 0);//SysTick_CTRL_ENABLE_Msk;//开始systick
    }
    else
        SysTick->CTRL &= ~(1UL << 0);//~SysTick_CTRL_ENABLE_Msk; //失能
}


static unsigned int cur_ms = 0;

void SysTick_Handler(void)
{
		s_systick.tick++;
			
		cur_ms++;
		if(cur_ms == s_systick.task_timeout)
		{
				cur_ms = 0;
			  if(s_systick.cur_task)
					s_systick.cur_task(s_systick.task_args);
		}
}


void Delay_us(u32 i)
{
	u32 temp;
	SysTick->LOAD=9*i;		 //
	SysTick->CTRL=0X01;		 //
	SysTick->VAL=0;		   	 //
	do
	{
		temp=SysTick->CTRL;		   //
	}
	while((temp&0x01)&&(!(temp&(1<<16))));	 //
	SysTick->CTRL=0;	//
	SysTick->VAL=0;		//
}


void Delay_ms(u32 i)
{
	u32 temp;
	SysTick->LOAD=9000*i;	  //设置重装值 72m
	SysTick->CTRL=0X01;		//使能
	SysTick->VAL=0;			//清零计数器
	do
	{
		temp=SysTick->CTRL;	   
	}
	while((temp&0x01)&&(!(temp&(1<<16))));	
	SysTick->CTRL=0;	//
	SysTick->VAL=0;		//
}

