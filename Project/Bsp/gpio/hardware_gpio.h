#ifndef HARDWARE_GPIO_H
#define HARDWARE_GPIO_H

#define PIN_GPIO_LOW        0x00
#define PIN_GPIO_HIGH       0x01 //高电平

#define PIN_GPIO_MODE_OUTPUT            0x00
#define PIN_GPIO_MODE_INPUT             0x01
#define PIN_GPIO_MODE_INPUT_PULLUP      0x02
#define PIN_GPIO_MODE_INPUT_PULLDOWN    0x03
#define PIN_GPIO_MODE_OUTPUT_OD         0x04 //开漏输出

#define PIN_GPIO_IRQ_RISING             0x00
#define PIN_GPIO_IRQ_FALLING            0x01
#define PIN_GPIO_IRQ_RISING_FALLING     0x02
#define PIN_GPIO_IRQ_HIGH_LEVEL         0x03
#define PIN_GPIO_IRQ_LOW_LEVEL          0x04

#define PIN_IRQ_DISABLE         0x00
#define PIN_IRQ_ENABLE          0x01


#endif // HARDWARE_GPIO_H
