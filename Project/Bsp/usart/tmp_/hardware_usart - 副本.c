#include "hardware_usart.h"

#include "stm32f4xx_usart.h"
#include "stm32f4xx_rcc.h"

#include "stdarg.h"
#include "stdio.h"

#define SERIAL_COM1_UART							USART1
#define SERIAL_COM1_NAME							"USART1"
#define SERIAL_COM1_CLOCK							RCC_APB2Periph_USART1

#define SERIAL_COM1_TX_PIN						GPIO_Pin_9  //����
#define SERIAL_COM1_TX_GPIO						GPIOA
#define SERIAL_COM1_TX_GPIO_CLOCK			RCC_AHB1Periph_GPIOA

#define SERIAL_COM1_TX_GPIO_SOURCE		GPIO_PinSource9			//����
#define SERIAL_COM1_TX_AF							GPIOA				//�������ŵ���


#define SERIAL_COM1_RX_PIN						GPIO_Pin_10
#define SERIAL_COM1_RX_GPIO						GPIOA
#define SERIAL_COM1_RX_GPIO_CLOCK			RCC_AHB1Periph_GPIOA

#define SERIAL_COM1_RX_GPIO_SOURCE		GPIO_PinSource10
#define SERIAL_COM1_RX_AF							GPIOA

#define SERIAL_COM1_GPIO_AF_USART			GPIO_AF_USART1

/* 232����Ҫ��������*/
#define SERIAL_COM1_CTRL_PIN					NULL
#define SERIAL_COM1_CTRL_CLOCK				NULL
#define SERIAL_COM1_CTRL_GPIO					NULL

#define SERIAL_COM1_IRQn							USART1_IRQChannel


/*            ��ʱֻ���������������Ҫ�����ӿ�   */

void hardware_usart_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	
	/* ʹ��gpio clock */
	RCC_AHB1PeriphClockCmd(SERIAL_COM1_TX_GPIO_CLOCK | SERIAL_COM1_RX_GPIO_CLOCK,ENABLE);
	
	/* ����1��Ӧ���Ÿ�λӳ��*/
	GPIO_PinAFConfig(SERIAL_COM1_TX_AF,SERIAL_COM1_TX_GPIO_SOURCE,SERIAL_COM1_GPIO_AF_USART);
	GPIO_PinAFConfig(SERIAL_COM1_RX_AF,SERIAL_COM1_RX_GPIO_SOURCE,SERIAL_COM1_GPIO_AF_USART);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;//����ģʽ
	GPIO_InitStruct.GPIO_Pin = SERIAL_COM1_TX_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP; //����
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP; //����
	
	GPIO_Init(SERIAL_COM1_TX_GPIO,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Pin = SERIAL_COM1_RX_PIN;
	
	GPIO_Init(SERIAL_COM1_TX_GPIO,&GPIO_InitStruct);
	
	/* ʹ�� USART ʱ��*/
	RCC_APB2PeriphClockCmd(SERIAL_COM1_CLOCK,ENABLE);
	
	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	
	USART_Init(SERIAL_COM1_UART,&USART_InitStruct);
	
	USART_Cmd(SERIAL_COM1_UART,ENABLE);//��������
	
	USART_ITConfig(SERIAL_COM1_UART,USART_IT_RXNE,ENABLE);//���Ĵ���DR����
	USART_ITConfig(SERIAL_COM1_UART,USART_IT_TC,ENABLE);//��/д�Ĵ���DR����
	USART_ClearFlag(SERIAL_COM1_UART,USART_FLAG_TC);//��շ�����ɱ�־
}

void USART1_IRQHandler(void)
{
	
}

int usart1_getc(void)
{
	int ch;
	if(SERIAL_COM1_UART->SR & USART_FLAG_RXNE)
	{
			ch = SERIAL_COM1_UART->DR & 0xff;
	}
	
	return ch;
}

void usart1_putc(unsigned char ch)
{
	while(RESET == USART_GetFlagStatus(SERIAL_COM1_UART,USART_FLAG_TXE))	;
	USART_SendData(SERIAL_COM1_UART,ch);
	while(RESET == USART_GetFlagStatus(SERIAL_COM1_UART,USART_FLAG_TC)) ;
	
}

void console_output(const char *str)
{
	while(*str)
	{
		if(*str == '\n')
		{
			usart1_putc('\r');
		}
		
		usart1_putc(*str++);
	}
}

#define SEND_BUFFER_MAX_LEN		128
char log_buf[SEND_BUFFER_MAX_LEN];
void printk_(const char *fmt,...)
{
	va_list args;
	
	va_start(args,fmt);
	
	vsnprintf(log_buf,sizeof(log_buf) - 1,fmt,args);
	
	console_output(log_buf);
	
	va_end(args);
}

#if 1
#pragma import(__use_no_semihosting)             
                
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
   
void _sys_exit(int x) 
{ 
	x = x; 
} 

int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0); 
	USART1->DR = (u8) ch;      
	return ch;
}
#endif
