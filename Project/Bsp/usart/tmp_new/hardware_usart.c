#include "stm32f4xx_usart.h"
#include "stm32f4xx_rcc.h"

#include "hardware_usart.h"

#include <string.h>

//for 485
typedef void (*rcc_cb_ptr)(int,int);

struct gpio_ctrl_pin
{
    rcc_cb_ptr rcc_ptr;
    unsigned int rcc;
    GPIO_TypeDef *gpio;
    unsigned int pin;
};
#define GPIO_PIN_CONFIG(rcc,gpio,gpio_index) { RCC_##rcc##PeriphClockCmd,RCC_##rcc##Periph_GPIO##gpio, GPIO##gpio, GPIO_Pin_##gpio_index}
#define GPIO_PIN_CONFIG_DEFAULT {0,0,0,0}

/** 声明 **/
void serial_init(struct serial_device *serial);
long serial_read(struct serial_device *serial,void *buffer,unsigned long size);
long serial_write(struct serial_device *serial,void *buffer,unsigned long size);
void serial_putc(struct serial_device *serial,char ch);
int serial_getc(struct serial_device *serial);
void serial_configure(struct serial_device *serial,struct serial_configure *config);
void serial_clear(struct serial_device *serial);
void serial_rt_ctrl(unsigned char no,unsigned char rt_stauts);//收发转换 485
void serial_enable_irq(struct serial_device *serial,unsigned char enabled);//0 禁止 1 使能

/** 全局变量 **/
struct UartCom
{
    USART_TypeDef *USARTx;
    IRQn_Type irq;
};
static struct UartCom systemUsart[UART_COM_MAX];

static struct gpio_ctrl_pin Usart_Ctrl_Pin[] =
{
#ifdef UART_COM1_USED
    GPIO_PIN_CONFIG_DEFAULT,
#endif
#ifdef UART_COM2_USED
    GPIO_PIN_CONFIG_DEFAULT,
#endif
#ifdef UART_COM3_USED
    GPIO_PIN_CONFIG_DEFAULT,
#endif
#ifdef UART_COM4_USED
    GPIO_PIN_CONFIG_DEFAULT,
#endif // UART_COM4_USED
#ifdef UART_COM5_USED
    //GPIO_PIN_CONFIG(AHB1,A,10),
    GPIO_PIN_CONFIG_DEFAULT,
#endif // UART_COM5_USED
};

/***** ****************************USART1 *************************/

#ifdef UART_COM1_USED

#define UART1_GPIO_TX               GPIO_Pin_9
#define UART1_TX_PIN_SOURCE         GPIO_PinSource9
#define UART1_TX_GPIO_RCC           RCC_AHB1Periph_GPIOA
#define UART1_TX_GPIO               GPIOA

#define UART1_GPIO_RX               GPIO_Pin_10
#define UART1_RX_PIN_SOURCE         GPIO_PinSource10
#define UART1_RX_GPIO_RCC           RCC_AHB1Periph_GPIOA
#define UART1_RX_GPIO               GPIOA

#define UART1_RCC                   RCC_APB2Periph_USART1
#define UART1_USART                 USART1

struct serial_tx_fifo tx_fifo =
{
    0
};

#define SERIAL_RX_BUFFER_LEN    256
unsigned char rx_buffer[SERIAL_RX_BUFFER_LEN];
struct serial_rx_fifo rx_fifo =
{
    rx_buffer,
    SERIAL_RX_BUFFER_LEN,
    0,
    0,
    0
};

struct serial_device UartCom1 =
{
    {
        UART_COM1, //USART1
        115200,
        USART_WordLength_8b,//8
        USART_StopBits_1,
        USART_Parity_No,
        USART_Mode_Rx | USART_Mode_Tx
    },
    3,//rx timeout 3ms
    &tx_fifo,
    &rx_fifo,
    serial_init,
    serial_read,
    serial_write,
    serial_putc,
    serial_getc,
    serial_configure,
    serial_clear,
    serial_rt_ctrl,
    serial_enable_irq
};

static void USART1_Configuration(struct serial_configure *param)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    /* Enable GPIO clock */
    RCC_AHB1PeriphClockCmd(UART1_TX_GPIO_RCC|UART1_RX_GPIO_RCC, ENABLE);

    //UART????
    GPIO_PinAFConfig(UART1_TX_GPIO,UART1_TX_PIN_SOURCE,GPIO_AF_USART1);
    GPIO_PinAFConfig(UART1_RX_GPIO,UART1_RX_PIN_SOURCE,GPIO_AF_USART1);

    /* Configure USART Tx as alternate function  */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = UART1_GPIO_TX;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
    GPIO_Init(UART1_TX_GPIO, &GPIO_InitStructure);

    /* Configure USART Rx as alternate function  */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = UART1_GPIO_RX;
    GPIO_Init(UART1_RX_GPIO, &GPIO_InitStructure);

    /* Enable UART clock */
    RCC_APB2PeriphClockCmd(UART1_RCC, ENABLE);

    /* USART configuration */
    USART_InitStructure.USART_BaudRate = param->baud_rate;
    USART_InitStructure.USART_WordLength = param->word_length;
    USART_InitStructure.USART_StopBits = param->stop_bits;
    USART_InitStructure.USART_Parity = param->parity;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//	USART_InitStructure.USART_Clock = USART_Clock_Disable;
//	USART_InitStructure.USART_CPOL = USART_CPOL_Low;
//	USART_InitStructure.USART_CPHA = USART_CPHA_1Edge;
//	USART_InitStructure.USART_LastBit = USART_LastBit_Disable;
    USART_Init(UART1_USART, &USART_InitStructure);              //Initializes the USARTx peripheral according to the specified parameters in the USART_InitStruct

    USART_Cmd(UART1_USART, ENABLE);                                              //Enables or disables the specified USART peripheral
    // Enable USART
    USART_ITConfig(UART1_USART, USART_IT_RXNE, ENABLE);
    USART_ITConfig(UART1_USART, USART_IT_TC, ENABLE);
    USART_ClearFlag(UART1_USART, USART_FLAG_TC);
}

#endif // UART_COM1_USED

static void UartCtrlGpio_Init(void)
{
    unsigned char i;
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

    for(i = 0; i < (sizeof(Usart_Ctrl_Pin) / sizeof(Usart_Ctrl_Pin[0])); i++)
    {
        if(Usart_Ctrl_Pin[i].rcc_ptr != 0)
        {
            Usart_Ctrl_Pin[i].rcc_ptr(Usart_Ctrl_Pin[i].rcc,ENABLE);

            GPIO_InitStruct.GPIO_Pin = Usart_Ctrl_Pin[i].pin;
            GPIO_Init(Usart_Ctrl_Pin[i].gpio,&GPIO_InitStruct);

            serial_rt_ctrl(i,Bit_RESET);
        }
    }
}

void serial_init(struct serial_device *serial)
{
    unsigned char no = serial->config.serial_no % UART_COM_MAX;
		struct UartCom *curUsart = &systemUsart[no];

    UartCtrlGpio_Init();
    switch(no)
    {
#ifdef UART_COM1_USED
    case UART_COM1:
        curUsart->USARTx = USART1;
        curUsart->irq = USART1_IRQn;
        USART1_Configuration(&serial->config);
        break;
#endif // UART_COM1_USED
    default:
        break;
    }
}

long serial_read(struct serial_device *serial,void *buffer,unsigned long length)
{
    unsigned long size = length;
    struct serial_rx_fifo *rx_fifo = serial->serial_rx;
    int ch = -1;
    unsigned char *data = (unsigned char *)buffer;

    while(length)
    {
        if(serial->rx_timeout <= 0)
        {
            if((rx_fifo->get_index == rx_fifo->put_index) && (rx_fifo->is_full == 0))
            {
                break; // no data
            }

            ch = rx_fifo->buffer[rx_fifo->get_index];
            rx_fifo->get_index += 1;
            if(rx_fifo->get_index >= rx_fifo->buffer_len)
            {
                rx_fifo->get_index = 0;
            }

            if(rx_fifo->is_full == 1)
            {
                rx_fifo->is_full = 0;
            }

            *data = ch & 0xff;
            data++;
            length--;
        }
    }

    return size - length;
}

long serial_write(struct serial_device *serial,void *buffer,unsigned long length)
{
    struct serial_tx_fifo *tx = serial->serial_tx;
    unsigned long size = length;
    unsigned char *ptr = (unsigned char *)buffer;

    while(length)
    {
        serial->putc(serial,*ptr);

        ptr++;
        length--;
    }

    return size - length;
}

void serial_putc(struct serial_device *serial,char ch)
{
    unsigned char no = serial->config.serial_no % UART_COM_MAX;

    struct UartCom *curUsart = &systemUsart[no];

    while(RESET == USART_GetFlagStatus(curUsart->USARTx,USART_FLAG_TXE))
				;

		USART_SendData(curUsart->USARTx,ch);

		while(RESET == USART_GetFlagStatus(curUsart->USARTx,USART_FLAG_TC))
				;
}

int serial_getc(struct serial_device *serial)
{
    struct UartCom *curUsart = &systemUsart[serial->config.serial_no];

    int ch = -1;
    if(USART_GetFlagStatus(curUsart->USARTx,USART_FLAG_RXNE) != RESET)
    {
        ch = (int)USART_ReceiveData(curUsart->USARTx);
    }

    return ch;
}

void serial_configure(struct serial_device *serial,struct serial_configure *config)
{
    unsigned char no = serial->config.serial_no % UART_COM_MAX;
    struct UartCom *curUsart = &systemUsart[no];
    switch(no)
    {
#ifdef UART_COM1_USED
    case UART_COM1:
        USART1_Configuration(config);
        break;
#endif // UART_COM1_USED
    default:
        break;
    }
}

void serial_clear(struct serial_device *serial)
{
    unsigned char no = serial->config.serial_no % UART_COM_MAX;

    switch(no)
    {
#ifdef UART_COM1_USED
    case UART_COM1:
        memset(serial->serial_rx->buffer,0x00,serial->serial_rx->buffer_len);
        serial->serial_rx->get_index = 0;
        serial->serial_rx->put_index = 0;
        serial->serial_rx->is_full = 0;
        break;
#endif // UART_COM1_USED
    }
}

void serial_rt_ctrl(unsigned char no,unsigned char rt_stauts)//收发转换 485
{
    switch(no)
    {
#ifdef UART_COM1_USED
    case UART_COM1:
        GPIO_WriteBit(Usart_Ctrl_Pin[no].gpio,Usart_Ctrl_Pin[no].pin,rt_stauts);
        break;
#endif
    default:
        break;
    }
}

void serial_enable_irq(struct serial_device *serial,unsigned char enabled)//0 禁止 1 使能
{
    unsigned char no = serial->config.serial_no % UART_COM_MAX;
    struct UartCom *curUart = &systemUsart[no];

    // Enable USART
    USART_ITConfig(curUart->USARTx, USART_IT_RXNE, enabled ? ENABLE : DISABLE);
    USART_ITConfig(curUart->USARTx, USART_IT_TC, enabled ? ENABLE : DISABLE);
    USART_ClearFlag(curUart->USARTx, USART_FLAG_TC);
}

static void usart_irq(struct serial_device *serial)
{
    struct UartCom *curUsart = &systemUsart[serial->config.serial_no];
    int ch = -1;

    //接收
    if(USART_GetITStatus(curUsart->USARTx,USART_IT_RXNE) != RESET)
    {
        serial->rx_timeout = 3; //3ms
        while(1)
        {
            ch = serial->getc(serial);
            if(ch == -1)
                break;

            serial->serial_rx->buffer[serial->serial_rx->put_index] = ch;
            serial->serial_rx->put_index += 1;
            if(serial->serial_rx->put_index >= serial->serial_rx->buffer_len)
            {
                serial->serial_rx->put_index = 0;
            }

            if(serial->serial_rx->put_index == serial->serial_rx->get_index)
            {
                serial->serial_rx->get_index += 1;
                serial->serial_rx->is_full = 1;//满了

                if(serial->serial_rx->get_index >= serial->serial_rx->buffer_len)
                {
                    serial->serial_rx->get_index = 0;
                }
            }

        }
        USART_ClearITPendingBit(curUsart->USARTx,USART_IT_RXNE);
    }
}

static void NVIC_Configuration(struct serial_device *uart)
{
    unsigned char no = uart->config.serial_no % UART_COM_MAX;
    struct UartCom *curUsart = &systemUsart[no];

    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the USART1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = curUsart->irq;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void USART1_IRQHandler()
{
#ifdef UART_COM1_USED
	usart_irq(&UartCom1);
#endif
}

void hardware_usart_init()
{
#ifdef UART_COM1_USED
    UartCom1.init(&UartCom1);
    NVIC_Configuration(&UartCom1);
#endif // UART_COM1_USED
}

void hardware_checkover()
{
#ifdef UART_COM1_USED
    if(UartCom1.rx_timeout > 0)
    {
        UartCom1.rx_timeout--;
    }
#endif // UART_COM1_USED
}
