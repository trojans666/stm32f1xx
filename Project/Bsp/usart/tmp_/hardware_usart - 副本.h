#ifndef SERIAL_PRINTF_H
#define SERIAL_PRINTF_H

void hardware_usart_init(void);
int usart1_getc(void);
void usart1_putc(unsigned char ch);

void printk_(const char *str,...);

#endif 

