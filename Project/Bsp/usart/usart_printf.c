#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"

#include "usart_printf.h"
#include <stdio.h>
#include <stdarg.h>

#define RS232_PORT						USART3
#define RS232_RCC							RCC_APB1Periph_USART3


typedef void (*rs232_gpio_rcc_cb)(unsigned int , FunctionalState);
struct rs232_gpio
{
	rs232_gpio_rcc_cb rcc_ptr;
	unsigned int rcc;
	GPIO_TypeDef *gpio;
	unsigned int pin;
	//unsigned char af_flag;// «∑Ò÷ÿ”≥…‰
};

#define RS232_GPIO_PIN_CONFIG(rcc,gpio,gpio_index) { RCC_##rcc##PeriphClockCmd,RCC_##rcc##Periph_GPIO##gpio, GPIO##gpio, GPIO_Pin_##gpio_index }

static struct rs232_gpio RS232_GPIO[] = 
{
	//tx
	RS232_GPIO_PIN_CONFIG(APB2,B,10),
	//rx
	RS232_GPIO_PIN_CONFIG(APB2,B,11)
};

void hardware_usart_printf_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;

	//gpio clock
	
	if(RS232_GPIO[0].rcc_ptr)
	{
		RS232_GPIO[0].rcc_ptr(RS232_GPIO[0].rcc,ENABLE);
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;//????
		GPIO_InitStruct.GPIO_Pin = RS232_GPIO[0].pin;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		
		GPIO_Init(RS232_GPIO[0].gpio,&GPIO_InitStruct);
	}
	
	if(RS232_GPIO[1].rcc_ptr)
	{
		RS232_GPIO[1].rcc_ptr(RS232_GPIO[0].rcc,ENABLE);
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStruct.GPIO_Pin = RS232_GPIO[1].pin;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		
		GPIO_Init(RS232_GPIO[1].gpio,&GPIO_InitStruct);
	}
	
	//usart
	RCC_APB1PeriphClockCmd(RS232_RCC,ENABLE);

	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	USART_Init(RS232_PORT,&USART_InitStruct);

	USART_Cmd(RS232_PORT,ENABLE);//????

	USART_ITConfig(RS232_PORT,USART_IT_RXNE,ENABLE);
	USART_ITConfig(RS232_PORT,USART_IT_TC,ENABLE);
	USART_ClearFlag(RS232_PORT,USART_FLAG_TC);
}

void usart_putc(unsigned char ch)
{
	while(RESET == USART_GetFlagStatus(RS232_PORT,USART_FLAG_TXE))	;
	USART_SendData(RS232_PORT,ch);
	while(RESET == USART_GetFlagStatus(RS232_PORT,USART_FLAG_TC)) ;
}

static void console_output(const char *str)
{
	while(*str)
	{
		if(*str == '\n')
		{
			usart_putc('\r');
		}

		usart_putc(*str++);
	}
}

#define SEND_BUFFER_MAX_LEN		128
static char log_buf[SEND_BUFFER_MAX_LEN];
void log_info(const char *fmt,...)
{
	va_list args;

	va_start(args,fmt);

	vsnprintf(log_buf,sizeof(log_buf) - 1,fmt,args);

	console_output(log_buf);

	va_end(args);
}
