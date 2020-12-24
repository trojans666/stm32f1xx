#include "stm32f10x_tim.h"
#include "hardware_pwm.h"
#include <string.h>

void pwm_init(struct pwm_device *pwm);
void pwm_enable(struct pwm_device *pwm);
void pwm_config(struct pwm_device *pwm,struct pwm_config *cfg);
void pwm_set_polarity(struct pwm_device *pwm,unsigned char polarity);
void pwm_set_compare(struct pwm_device *pwm,unsigned int value);

typedef void (*compare_cb)(TIM_TypeDef *,unsigned int );
struct pwm_port
{
    TIM_TypeDef *TIMx;
    compare_cb set_compare;
};
static struct pwm_port systemPWM[PWM_PORT_MAX];

#ifdef PWM_PORT0_USED

struct pwm_device pwm14 =
{
    PWM_PORT_0,
    {
        1,0,0
    },
    pwm_init,
    pwm_enable,
    pwm_config,
    pwm_set_polarity,
    pwm_set_compare
};

void PWM_Configure(struct pwm_config *config)
{
    GPIO_InitTypeDef GPIO_InitStructure;   //声明一个结构体变量，用来初始化GPIO

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;//声明一个结构体变量，用来初始化定时器

	TIM_OCInitTypeDef TIM_OCInitStructure;//根据TIM_OCInitStruct中指定的参数初始化外设TIMx

	/* 开启时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_Init(GPIOC,&GPIO_InitStructure);


	//TIM3定时器初始化
	TIM_TimeBaseInitStructure.TIM_Period = 900;	   //不分频,PWM 频率=72000/900=8Khz//设置自动重装载寄存器周期的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 0;//设置用来作为TIMx时钟频率预分频值，100Khz计数频率
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM向上计数模式
	TIM_TimeBaseInit(TIM3, & TIM_TimeBaseInitStructure);

	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3,ENABLE);//改变指定管脚的映射	//pC7

	//PWM初始化	  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;//PWM输出使能
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_Low;

	TIM_OC2Init(TIM3,&TIM_OCInitStructure);
	//注意此处初始化时TIM_OC2Init而不是TIM_OCInit，否则会出错。因为固件库的版本不一样。

	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);//使能或者失能TIMx在CCR2上的预装载寄存器
	
	TIM_Cmd(TIM3,ENABLE);//使能或者失能TIMx外设
}
#endif // PWM_PORT0_USED

void pwm_init(struct pwm_device *pwm)
{
    unsigned char no = pwm->pwm_id % PWM_PORT_MAX;
    struct pwm_port *curPort = &systemPWM[no];
    switch(no)
    {
#ifdef PWM_PORT0_USED
    case PWM_PORT_0:
        curPort->TIMx = TIM3;
        curPort->set_compare = TIM_SetCompare1;//不同的tim用的不一样
        PWM_Configure(&pwm->config);
        break;
#endif // PWM_PORT0_USED
    default:
        break;
    }
}

void pwm_enable(struct pwm_device *pwm)
{
    unsigned char no = pwm->pwm_id % PWM_PORT_MAX;
    struct pwm_port *curPort = &systemPWM[no];

    TIM_Cmd(curPort->TIMx,ENABLE);
}

void pwm_config(struct pwm_device *pwm,struct pwm_config *config)
{
    unsigned char no = pwm->pwm_id % PWM_PORT_MAX;
//    struct pwm_port *curPort = &systemPWM[no];
    switch(no)
    {
#ifdef PWM_PORT0_USED
    case PWM_PORT_0:
        memcpy(&pwm->config,config,sizeof(struct pwm_config));
        break;
#endif // PWM_PORT0_USED
    default:
        break;
    }
}

void pwm_set_polarity(struct pwm_device *pwm,unsigned char polarity)
{
    unsigned char no = pwm->pwm_id % PWM_PORT_MAX;
    struct pwm_port *curPort = &systemPWM[no];

    TIM_OC1NPolarityConfig(curPort->TIMx,(polarity == 0 ? 0 : 1));
}

void pwm_set_compare(struct pwm_device *pwm,unsigned int value)
{
    unsigned char no = pwm->pwm_id % PWM_PORT_MAX;
    struct pwm_port *curPort = &systemPWM[no];

    curPort->set_compare(curPort->TIMx,value);
}

void hardware_pwm_init()
{
#ifdef PWM_PORT0_USED
	pwm14.init(&pwm14);
#endif
}

