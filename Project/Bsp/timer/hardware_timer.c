#include "stm32f4xx_tim.h"
#include "hardware_timer.h"

#include <stdio.h>


#ifdef TIMER2_USED

//timer2µÄÅäÖÃ
#define TIMER_PORT2_TIMx    TIM2
#define TIMER_PORT2_NAME    "TIMER2"

void Timer2_Configuration(struct timer_config *config)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
	/* timer */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

    // Tout = ((period + 1) * (prescaler + 1)) / 84000000 (84MHz)

	TIM_TimeBaseStruct.TIM_Period = (config->ms_reload * 1000) - 1;
	TIM_TimeBaseStruct.TIM_Prescaler = 84 - 1;
	TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;//不分频
	TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStruct);

	TIM_ClearFlag(TIM2,TIM_FLAG_Update);
}

#endif // TIMER2_USED

typedef struct timer_port_
{
    TIM_TypeDef *TIMx;
    IRQn_Type irq;
}timer_port_t;

static timer_port_t systemTimer[TIMER_PORT_MAX];

void inn_timer_init(struct timer_ *timer)
{
    unsigned int no = timer->timer_id % TIMER_PORT_MAX;
    timer_port_t *timer_port = &systemTimer[no];
    switch(no)
    {
#ifdef TIMER2_USED
    case TIMER_PORT_2:
        timer_port->TIMx = TIMER_PORT2_TIMx;
        timer_port->irq = TIM2_IRQn;
        Timer2_Configuration(&timer->config);
        break;
#endif
    default:
        break;
    }
}

void inn_timer_reinit(struct timer_ *timer)
{
    inn_timer_init(timer);
}
//目前为1个timer做一个任务
void inn_timer_register_callback(struct timer_ *timer,handle_timer_cb func)
{
    if(timer)
    {
        timer->cb_ptr = func;
    }
}

void inn_timer_unregister_callback(struct timer_ *timer)
{
    if(timer)
    {
        timer->cb_ptr = NULL;
    }
}

void inn_timer_start(timer_t *timer)
{
    if(timer)
    {
        unsigned int no = timer->timer_id % TIMER_PORT_MAX;
        timer_port_t *timer_port = &systemTimer[no];

        TIM_ITConfig(timer_port->TIMx,TIM_IT_Update,ENABLE);
        TIM_Cmd(timer_port->TIMx,ENABLE);
    }
}

void inn_timer_stop(timer_t *timer)
{
    if(timer)
    {
        unsigned int no = timer->timer_id % TIMER_PORT_MAX;
        timer_port_t *timer_port = &systemTimer[no];

        TIM_ITConfig(timer_port->TIMx,TIM_IT_Update,DISABLE);
        TIM_Cmd(timer_port->TIMx,DISABLE);
    }
}

void inn_timer_irq(timer_t *timer)
{
    if(timer)
    {
        unsigned int no = timer->timer_id % TIMER_PORT_MAX;
        timer_port_t *timer_port = &systemTimer[no];

        if(TIM_GetITStatus(timer_port->TIMx,TIM_IT_Update) != RESET)
        {
            if(timer->cb_ptr)
                timer->cb_ptr();

            TIM_ClearITPendingBit(timer_port->TIMx,TIM_FLAG_Update);
        }
    }
}

void inn_timer_config(timer_t *timer,struct timer_config *config)
{
    if(timer && config)
    {
        timer->config.ms_reload = config->ms_reload;
    }
}


void TIM2_IRQHandler(void)
{
#ifdef TIMER2_USED
    timer2.timer_irq(&timer2);
#endif // TIMER2_USED
}


#define TIMER2_USED 1

#ifdef TIMER2_USED

timer_t timer2 =
{
    TIMER_PORT_2,
    NULL,
    {
        1  //1 ms
    },
    inn_timer_init,
    inn_timer_reinit,
    inn_timer_register_callback,
    inn_timer_unregister_callback,
    inn_timer_start,
    inn_timer_stop,
    inn_timer_irq,
    inn_timer_config
};

#endif // TIMER2_USED

void NVIC_Configuration(struct timer_ *timer,unsigned char sub_pri)
{
    unsigned char no = timer->timer_id % TIMER_PORT_MAX;
    timer_port_t *curTimer = &systemTimer[no];

    NVIC_InitTypeDef NVIC_InitStruct;


	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_InitStruct.NVIC_IRQChannel = curTimer->irq;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = sub_pri;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStruct);
}

void hardware_timer_init()
{
    unsigned char sub_pri = 3;
#ifdef TIMER2_USED
    timer2.timer_init(&timer2);
    NVIC_Configuration(&timer2,sub_pri);
#endif // TIMER2_USED
}
