#ifndef USART_PRINTF_H
#define USART_PRINTF_H



void hardware_usart_printf_init(void);

void log_info(const char *fmt,...);

void usart_putc(unsigned char ch);




#endif // USART_PRINTF_h
