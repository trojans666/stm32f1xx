#ifndef HARDWARE_USART_H
#define HARDWARE_USART_H

#include "usart_queue.h"

typedef enum
{
    UART_COM1,
    UART_COM2,
    UART_COM3,
    UART_COM4,
    UART_COM5,
    UART_COM_MAX
}serial_port_e;

//串口配置参数
struct serial_configure
{
    unsigned char serial_no;//串口号
    unsigned int baud_rate;
    unsigned short word_length;
    unsigned short stop_bits;
    unsigned short parity;
    unsigned short mode;//usart_mode_rx  usart_mode_tx
};



struct serial_device
{
    struct serial_configure config;
    unsigned char rx_timeout;//接收多长算一帧
    queue_t *serial_tx;
    queue_t *serial_rx;

    void (*init)(struct serial_device *serial);
    long (*read)(struct serial_device *serial,void *buffer,unsigned long size);
    long (*write)(struct serial_device *serial,void *buffer,unsigned long size);
    void (*putc)(struct serial_device *serial,char ch);
    int (*getc)(struct serial_device *serial);
    void (*configure)(struct serial_device *serial,struct serial_configure *config);
    void (*clear)(struct serial_device *serial);
    void (*rt_ctrl)(unsigned char no,unsigned char rt_stauts);//收发转换 485
    void (*enable_irq)(struct serial_device *serial,unsigned char enabled);//0 禁止 1 使能
};

#define UART_COM1_USED	1

#ifdef UART_COM1_USED
    extern struct serial_device UartCom1;
#endif

void hardware_usart_init(void);
void hardware_checkover();

#endif // HARDWARE_USART_H
