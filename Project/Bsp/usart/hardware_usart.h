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
    UART_COMNUM
}serial_port_e;



typedef struct
{
    volatile unsigned char b0:1;
    volatile unsigned char b1:1;
    volatile unsigned char b2:1;
    volatile unsigned char b3:1;
    volatile unsigned char b4:1;
    volatile unsigned char b5:1;
    volatile unsigned char b6:1;
    volatile unsigned char b7:1;
}byte_field;

typedef struct
{
    unsigned char No; //串口号
    unsigned int baudRate;
    unsigned short WordLength;
    unsigned short stopBits;
    unsigned short parity;
    unsigned short mode; //USART_Mode_Rx USART_Mode_Tx
}serial_params_t;

typedef struct serial_
{
    serial_params_t param;
    byte_field flg; //b0 txbusy b1 rxbusy
    unsigned char RxOverTime;//??????
    unsigned char TxErrTime;
    queue_t     *TxList;
    unsigned int TxBufferLen;
    queue_t     *RxList;
    unsigned int RxBufferLen;

    void (*init)(struct serial_ *serial);
    void (*configure)(unsigned char No,serial_params_t *params);
	unsigned int (*read)(struct serial_ *serial, void *pRecvBuf, unsigned int len);	//??n???
	void (*write)(struct serial_ *serial, void *pSendBuf, unsigned int len);
	void (*rt_ctrl)(unsigned char No, unsigned char rtStatus);//for 485 rx_tx_switch
	void (*irq)(struct serial_ *serial);
	void (*checkover)(struct serial_ *serial);
}serial_t;

#define UART_COM1_USED      1

#ifdef UART_COM1_USED
		extern serial_t UartCom1;
#endif

void hardware_usart_init(void);
void hardware_checkover(void);

#endif
