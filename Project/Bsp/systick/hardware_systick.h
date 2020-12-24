#ifndef HARDWARE_SYSTICK_H
#define HARDWARE_SYSTICK_H

typedef void (*task_ptr)(void *args);
struct systick_dev
{
	 unsigned char max_hz;
	 unsigned int tick;
	 volatile unsigned int per_ms;
	 task_ptr cur_task;
	 void *task_args;
	 unsigned int task_timeout;
	
	 void (*register_task)(unsigned int timeout,void (*task)(void *args),void *args);
	 void (*unregister_task)();
	
	 void (*set_reload_value)(unsigned int ms);
	 unsigned int (*get_reload_value)(void);
	
	 unsigned int (*get_cur_value)(void);
	
	 void (*irq_enable)(void);
	 void (*irq_disable)(void);
	
	 unsigned int (*get_tick)();
#define NO_OS_USED		1
#ifdef NO_OS_USED
	 void (*systick_delay_ms)(unsigned int ms);
#else
	 void (*os_delay_ms)(unsigned int ms);
#endif 
};

/**≥ı ºªØ**/
extern struct systick_dev s_systick;
void hardware_systick_init(void);

void Delay_us(unsigned int i);
void Delay_ms(unsigned int i);
#endif


