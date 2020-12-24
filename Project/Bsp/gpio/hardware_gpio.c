#include "stm32f4xx_gpio.h"

#include "hardware_gpio.h"

#define STM32_PIN__(rcc,gpio,gpio_index) { RCC_##rcc##PeriphClockCmd ,RCC_##rcc##Periph_GPIO##gpio, GPIO##gpio, GPIO_Pin_##gpio_index}
#define STM32_PIN_DEFAULT {0,0,0}

#define ARRAY_LEN(items) (sizeof(items) / sizeof(items[0]))

typedef void (*rcc_cb_ptr)(uint32_t,FunctionalState);
struct gpio_pin
{
    rcc_cb_ptr rcc_ptr;
    unsigned int rcc;
    GPIO_TypeDef *gpio;
    unsigned int pin;
};

struct gpio_irq
{
    unsigned char port_source;//EXTI_PortSourceGPIOA
    unsigned char pin_source; //EXTI_PinSource0
    enum IRQn irq_exti_channel;//EXTI0_IRQn
    unsigned int exti_line;//EXTI_Line0
};

static const struct gpio_pin pins_A[] =
{

};

static const struct gpio_pin pins_F[] =
{
    STM32_PIN__(AHB1,F,9),
    STM32_PIN__(AHB1,F,10)
};

struct gpio_pin *get_pin_F(int pin)
{
    const struct gpio_pin *index = (0);
    int pos = 0;
    for(pos = 0;pos < ARRAY_LEN(pins_F);pos++)
    {
        if(index->pin == pin)
            return index;
    }

    return index;
};


struct gpio_pin_irq_hdr
{
    int                 pin;
    unsigned int        mode;
    void (*hdr)(void *args);
    void             *args;
};

static struct gpio_pin_irq_hdr pin_irq_tab[] =
{
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0}   //16个 每个组有16个引脚
};


int bit2bitno(unsigned int bit)
{
    int i;
    for (i = 0; i < 32; i++)
    {
        if ((1UL << i) == bit)
        {
            return i;
        }
    }
    return -1;
}

int bitno2bit(unsigned int bitno)
{
    if (bitno <= 32)
    {
        return 1UL << bitno;
    }
    else
    {
        return 0;
    }
}

static const struct gpio_irq *get_pin_irq(unsigned short pin)
{
    static struct gpio_irq irq;
    const struct gpio_pin *index;

    index = get_pin(pin);
    if (index == (0))
    {
        return (0);
    }

    irq.exti_line = index->pin;
    irq.pin_source = bit2bitno(index->pin);
    irq.port_source = ((uint32_t)index->gpio - GPIOA_BASE) / (GPIOB_BASE - GPIOA_BASE);
    switch (irq.pin_source)
    {
    case 0 : irq.irq_exti_channel = EXTI0_IRQn;break;
    case 1 : irq.irq_exti_channel = EXTI1_IRQn;break;
    case 2 : irq.irq_exti_channel = EXTI2_IRQn;break;
    case 3 : irq.irq_exti_channel = EXTI3_IRQn;break;
    case 4 : irq.irq_exti_channel = EXTI4_IRQn;break;
    case 5 :
    case 6 :
    case 7 :
    case 8 :
    case 9 : irq.irq_exti_channel = EXTI9_5_IRQn;break;
    case 10 :
    case 11 :
    case 12 :
    case 13 :
    case 14 :
    case 15 : irq.irq_exti_channel = EXTI15_10_IRQn;break;
    default : return (0);
    }

    return &irq;
};

//设置gpio工作模式
void gpio_setmode(unsigned int pin,unsigned char mode)
{
    const struct gpio_pin *index = (0);
    GPIO_InitTypeDef GPIO_InitStructure;

    index = get_pin(pin);
    if(index == (0))
    {
        return ;
    }
    // RCC_AHB1PeriphClockCmd(index->rcc,ENABLE);
    index->rcc_ptr(index->rcc,ENABLE);

    GPIO_InitStructure.GPIO_Pin = index->pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

    if (mode == PIN_GPIO_MODE_OUTPUT)
    {
        /* output setting */
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    }
    else if (mode == PIN_GPIO_MODE_INPUT)
    {
        /* input setting: not pull. */
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    }
    else if (mode == PIN_GPIO_MODE_INPUT_PULLUP)
    {
        /* input setting: pull up. */
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    }
    else if (mode == PIN_GPIO_MODE_INPUT_PULLDOWN)
    {
        /* input setting: pull down. */
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    }
    else if (mode == PIN_GPIO_MODE_OUTPUT_OD)
    {
        /* output setting: open drain */
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    }
    else
    {
        /* error mode */
       return ;
    }
    GPIO_Init(index->gpio, &GPIO_InitStructure);
}

//写
void gpio_write(unsigned int pin,int value)
{
    const struct gpio_pin *index = (0);
    index = get_pin(pin);
    if(index == (0))
    {
        return ;
    }
    if(value == PIN_GPIO_LOW)
    {
        GPIO_ResetBits(index->gpio,index->pin);
    }
    else
    {
        GPIO_SetBits(index->gpio,index->pin);
    }
}

int gpio_read(unsigned int pin)
{
    int value = PIN_GPIO_LOW;
    const struct gpio_pin *index = (0);

    index = get_pin(pin);
    if(index == (0))
    {
        return value;
    }

    if(GPIO_ReadInputDataBit(index->gpio,index->pin) == Bit_RESET)
    {
        value = PIN_GPIO_LOW;
    }
    else
    {
        value = PIN_GPIO_HIGH;
    }

    return value;
}
//注册中断
void gpio_attach_irq(unsigned int pin,unsigned char mode,
                     void (*func)(void *args),void *args)
{
    const struct gpio_index *index = (0);
    int level;
    int irqindex = -1;

    index = get_pin(pin);
    if(index == (0))
    {
        return ;
    }

    irqindex = bit2bitno(index->pin);
    if(irqindex < 0 || irqindex >= ARRAY_LEN(pin_irq_tab))
    {
        return ;
    }

    if(pin_irq_tab[irqindex].pin == pin &&
       pin_irq_tab[irqindex].hdr == func &&
       pin_irq_tab[irqindex].mode = mode &&
       pin_irq_tab[irqindex].args = args)
    {
        return ;
    }

    pin_irq_tab[irqindex].pin == pin;
    pin_irq_tab[irqindex].hdr == func;
    pin_irq_tab[irqindex].mode = mode;
    pin_irq_tab[irqindex].args = args;
}

//注销中断
void gpio_detach_irq(unsigned int pin)
{
    const struct pin_index *index;
    int level;
    int irqindex = -1;

    index = get_pin(pin);
    if (index == (0))
    {
        return ;
    }
    irqindex = bit2bitno(index->pin);
    if (irqindex < 0 || irqindex >= ARRAY_LEN(pin_irq_tab))
    {
        return -RT_ENOSYS;
    }

    if (pin_irq_tab[irqindex].pin == -1)
    {
        return ;
    }
    pin_irq_tab[irqindex].pin  = -1;
    pin_irq_tab[irqindex].hdr  = (0);
    pin_irq_tab[irqindex].mode = 0;
    pin_irq_tab[irqindex].args = (0);
}

void gpio_irq_enable(unsigned int pin,unsigned char enabled);
