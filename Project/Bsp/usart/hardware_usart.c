#include "hardware_usart.h"

#include "stm32f4xx_usart.h"
#include "stm32f4xx_rcc.h"

#include <stdio.h>
#include <stdarg.h>

//for 485
#define UART_TX		Bit_SET
#define UART_RX		Bit_RESET

#define UART_TXERR_TIME  10
#define UART_RXOVER_TIME  3

typedef struct
{
    unsigned int rcc;
    GPIO_TypeDef *gpio;
    unsigned short pin;
}GPIO_INFO_STRUCT;

void UartCom_Initialize(struct serial_ *baseCfg);
void UartCom_IRQ(struct serial_ *serial);
unsigned int UartCom_GetMsg(struct serial_ *serial, void *pRecvBuf, unsigned int len);
void UartCom_SendMsg(struct serial_ *serial, void *pSendBuf, unsigned int len);
void UartCom_RtCtrl(u8 No,u8 rtStatus);
void UartCtrlGpio_Init(void);
void UartCom_SetParmas(unsigned char No,serial_params_t *params);
void UartCom_CheckRxOver(serial_t *serial);

#define UART_COM1_USED 1
/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Configures the USART1.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#ifdef UART_COM1_USED
//串口1的配置
#define UART_COM1_UART					 	USART1          //USART_TypeDef 串口结构体
#define UART_COM1_NAME                   	"USART1"
#define UART_COM1_CLK                    	RCC_APB2Periph_USART1 //串口对应的rcc时钟

#define UART_COM1_TX_PIN                 	GPIO_Pin_9      //对应引脚
#define UART_COM1_TX_GPIO				 	GPIOA   //GPIO_TypeDef GPIO结构体
#define UART_COM1_TX_GPIO_CLK            	RCC_AHB1Periph_GPIOA //gpio时钟
#define UART_COM1_TX_SOURCE              	GPIO_PinSource9 //复用引脚
#define UART_COM1_TX_AF                  	GPIOA

#define UART_COM1_RX_PIN                 	GPIO_Pin_10
#define UART_COM1_RX_GPIO          		 	GPIOA
#define UART_COM1_RX_GPIO_CLK           	RCC_AHB1Periph_GPIOA
#define UART_COM1_RX_SOURCE              	GPIO_PinSource10
#define UART_COM1_RX_AF                  	GPIOA
#define UART_COM1_IRQn                   	USART1_IRQChannel//USART1_IRQn

#define UART_CTRL_NUMBER		UART_COMNUM
#define UART_COM1_CTRL_CLK		NULL
#define UART_COM1_CTRL_GPIO		NULL
#define UART_COM1_CTRL_PIN		NULL


void USART1_Configuration(serial_params_t *param)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(UART_COM1_RX_GPIO_CLK|UART_COM1_TX_GPIO_CLK, ENABLE);

	//UART????
	GPIO_PinAFConfig(UART_COM1_RX_GPIO,UART_COM1_RX_SOURCE,GPIO_AF_USART1);
	GPIO_PinAFConfig(UART_COM1_TX_GPIO,UART_COM1_TX_SOURCE,GPIO_AF_USART1);

	/* Configure USART Tx as alternate function  */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = UART_COM1_TX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(UART_COM1_TX_GPIO, &GPIO_InitStructure);

	/* Configure USART Rx as alternate function  */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = UART_COM1_RX_PIN;
	GPIO_Init(UART_COM1_RX_GPIO, &GPIO_InitStructure);

	/* Enable UART clock */
	RCC_APB2PeriphClockCmd(UART_COM1_CLK, ENABLE);

	/* USART configuration */
	USART_InitStructure.USART_BaudRate = param->baudRate;
	USART_InitStructure.USART_WordLength = param->WordLength;
	USART_InitStructure.USART_StopBits = param->stopBits;
	USART_InitStructure.USART_Parity = param->parity;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//	USART_InitStructure.USART_Clock = USART_Clock_Disable;
//	USART_InitStructure.USART_CPOL = USART_CPOL_Low;
//	USART_InitStructure.USART_CPHA = USART_CPHA_1Edge;
//	USART_InitStructure.USART_LastBit = USART_LastBit_Disable;
	USART_Init(UART_COM1_UART, &USART_InitStructure);              //Initializes the USARTx peripheral according to the specified parameters in the USART_InitStruct

	USART_Cmd(UART_COM1_UART, ENABLE);                                              //Enables or disables the specified USART peripheral
	// Enable USART
	USART_ITConfig(UART_COM1_UART, USART_IT_RXNE, ENABLE);
	USART_ITConfig(UART_COM1_UART, USART_IT_TC, ENABLE);
	USART_ClearFlag(UART_COM1_UART, USART_FLAG_TC);

	//BSP_IntVectSet(BSP_INT_ID_USART1, USART1_IRQHandler);
	//BSP_IntEn(BSP_INT_ID_USART1);
}
#endif


struct UartCom
{
    USART_TypeDef *USARTx; //发送接收用的都是这个结构体
    IRQn_Type irq;
};

static struct UartCom systemUsart[UART_COMNUM];

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


#ifdef UART_COM1_USED

#define COM1_TX_LEN     256
#define COM1_RX_LEN     256
static unsigned char  UartCom1Txbuf[COM1_TX_LEN];
static unsigned char  UartCom1Rxbuf[COM1_RX_LEN];

queue_t     UartCom1TxList;
queue_t     UartCom1RxList;

serial_t UartCom1 =
{
    {UART_COM1,	//USART1
	115200,
	USART_WordLength_8b,//8,
	USART_StopBits_1,	//1,
	USART_Parity_No,	//0,
	USART_Mode_Rx | USART_Mode_Tx},
	{0},
	0,
	0,
	&UartCom1TxList,
	COM1_TX_LEN,
	&UartCom1RxList,
	COM1_RX_LEN,
	UartCom_Initialize,
	UartCom_SetParmas,
	UartCom_GetMsg,
	UartCom_SendMsg,
	UartCom_RtCtrl,
	UartCom_IRQ,
	UartCom_CheckRxOver
};

#endif // UART_COM1_USED

void UartCom_Initialize(struct serial_ *baseCfg)
{
	u8 No = baseCfg->param.No%UART_COMNUM;
	struct UartCom *curUsart = &systemUsart[No];
	UartCtrlGpio_Init();
	switch(No)
	{
#ifdef UART_COM1_USED
		case UART_COM1:
			curUsart->USARTx = UART_COM1_UART;
			curUsart->irq = USART1_IRQn;
			//Q_INIT_BUF(UartCom1TxList, UartChar, UartCom1Txbuf, COM1_TX_LEN);
			//Q_INIT_BUF(UartCom1RxList, UartChar, UartCom1Rxbuf, COM1_RX_LEN);
			queue_init(&UartCom1TxList,sizeof(unsigned char),UartCom1Txbuf,COM1_TX_LEN);
			queue_init(&UartCom1RxList,sizeof(unsigned char),UartCom1Rxbuf,COM1_RX_LEN);
			USART1_Configuration(&baseCfg->param); //modbus????????
			break;
#endif
		default:
			break;
	}
}

void UartCom_SetParmas(unsigned char No,serial_params_t *params)
{
	switch(No)
	{
#ifdef UART_COM1_USED
		case UART_COM1:
			USART1_Configuration(params);
			break;
#endif
		default:
			break;
	}
}

void UartCom_IRQ(serial_t *serial)
{
	unsigned char dat;
	struct UartCom *curUsart = &systemUsart[serial->param.No];
	if (USART_GetITStatus(curUsart->USARTx, USART_IT_RXNE) != RESET) //USART_IT_RXNE 接收中断标志
	{
		USART_ClearITPendingBit(curUsart->USARTx, USART_IT_RXNE);
        dat = (unsigned char)USART_ReceiveData(curUsart->USARTx);
		serial->RxList->queue_push(serial->RxList,&dat);
		serial->RxOverTime = UART_RXOVER_TIME;
	}
	if(USART_GetITStatus(curUsart->USARTx, USART_IT_TC) != RESET)
	{
		USART_ClearITPendingBit(curUsart->USARTx, USART_IT_TC);
		if(queue_true==serial->TxList->queue_pop(serial->TxList,&dat,0))
		{
			USART_SendData(curUsart->USARTx, dat);
			serial->TxErrTime = UART_TXERR_TIME;
		}
		else
		{
			serial->TxErrTime = 0;
			serial->flg.b0 = 0;
			serial->rt_ctrl(serial->param.No,UART_RX); //switch to read mode
		}
	}
	USART_ClearITPendingBit(curUsart->USARTx, USART_IT_LBD);


}

//for 485
#if 0
const GPIO_INFO_STRUCT UART_CTRL_INFO_TAB[UART_CTRL_NUMBER] = {
	{UART_COM1_CTRL_CLK,UART_COM1_CTRL_GPIO,UART_COM1_CTRL_PIN}
};
#endif
const struct gpio_ctrl_pin UART_CTRL_INFO_TAB[UART_CTRL_NUMBER] =
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


void UartCtrlGpio_Init(void)
{
    #if 0
    u8 i;
    GPIO_InitTypeDef  GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    for(i=0;i<UART_CTRL_NUMBER;i++)
    {
    	if(UART_CTRL_INFO_TAB[i].rcc!=NULL)
		{
        	RCC_APB2PeriphClockCmd( UART_CTRL_INFO_TAB[i].rcc , ENABLE);
        	GPIO_InitStructure.GPIO_Pin = UART_CTRL_INFO_TAB[i].pin;
        	GPIO_Init(UART_CTRL_INFO_TAB[i].gpio, &GPIO_InitStructure);
            UartCom_RtCtrl(i,UART_RX);//???????
		}
    }
    #endif

    u8 i;
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    for(i = 0; i < UART_CTRL_NUMBER; i++)
    {
        if(UART_CTRL_INFO_TAB[i].rcc_ptr != 0)
        {
            UART_CTRL_INFO_TAB[i].rcc_ptr(UART_CTRL_INFO_TAB[i].rcc,ENABLE);

            GPIO_InitStructure.GPIO_Pin = UART_CTRL_INFO_TAB[i].pin;
            GPIO_Init(UART_CTRL_INFO_TAB[i].gpio,&GPIO_InitStructure);

            UartCom_RtCtrl(i,Bit_RESET);//接收
        }
    }

}
// for 485 control switch read/write direc
void UartCom_RtCtrl(u8 No,u8 rtStatus)
{
#if 0
	if(No>=UART_CTRL_NUMBER)
		return;
	//if(No==UART_COM1)
	//	LedGpio_OnOff(LEDB_R,rtStatus);
	if(UART_CTRL_INFO_TAB[No].rcc==NULL)
		return;
	GPIO_WriteBit(UART_CTRL_INFO_TAB[No].gpio,UART_CTRL_INFO_TAB[No].pin,(BitAction)rtStatus);
#endif
    if(No >= UART_CTRL_NUMBER)
        return ;
    if(UART_CTRL_INFO_TAB[No].rcc == 0)
        return ;
    GPIO_WriteBit((GPIO_TypeDef *)UART_CTRL_INFO_TAB[No].gpio,(uint16_t)UART_CTRL_INFO_TAB[No].pin,(BitAction)rtStatus);
}

void UartCom_SendMsg(serial_t *serial, void *pSendBuf, u32 len)
{
	unsigned char *ptr = (unsigned char *)pSendBuf;
	unsigned char dat;
	struct UartCom *curUsart = &systemUsart[serial->param.No];

	if(NULL == pSendBuf)
		return;
	if(serial->flg.b1) //??????
	{
		serial->flg.b1 = 0;
		serial->flg.b0 = 0;
		//DEBUGMSG("uart err\r\n");
	}
	while(len--)
	{
		serial->TxList->queue_push(serial->TxList,ptr++);
	}
	//if((0==serial->flg.b0)||(serial->flg.b1)) //??????

	if(0==serial->flg.b0)
	{
		serial->rt_ctrl(serial->param.No,UART_TX);
		serial->flg.b0 = 1;
		serial->flg.b1 = 0;
		serial->TxErrTime = UART_TXERR_TIME;
		serial->TxList->queue_pop(serial->TxList,&dat,0);
		USART_SendData(curUsart->USARTx, dat);
	}
}

unsigned int UartCom_GetMsg(struct serial_ *serial, void *pRecvBuf, u32 len)	//?len???
{
	u32 n=0;
	//struct UartCom *curUsart = &systemUsart[serial->param.No];
	if(NULL == pRecvBuf)
		return 0;
	while((serial->RxList->queue_is_empty(serial->RxList) != queue_true)&&(len--)&&(serial->RxOverTime==0))
	{
		serial->RxList->queue_pop(serial->RxList,(((unsigned char *)pRecvBuf)+n),0);
		n++;
	}
	return n;
}

unsigned int UartCom_TraverseMsg(serial_t *serial, void *pRecvBuf, unsigned int len)
{
	unsigned int n=0;
	//struct UartCom *curUsart = &systemUsart[serial->param.No];
	if(NULL == pRecvBuf)
		return 0;
	if((serial->RxList->queue_is_empty(serial->RxList) != queue_true)&&(serial->RxOverTime==0))
	{
		n = serial->RxList->queue_traverse(serial->RxList,pRecvBuf,len);
	}
	return n;
}

void UartCom_ClearRecMsg(serial_t *serial)
{
	serial->RxList->queue_clear(serial->RxList);
}

//为了确定何时算接收完一帧。要么每次读的时候 禁止中断。要么定时计数
void UartCom_CheckRxOver(serial_t *serial)
{
	if(NULL == serial)
		return;
	if(serial->RxOverTime > 0)
	{
		serial->RxOverTime--;
	}
	if(serial->flg.b0)
	{
		if(serial->TxErrTime--==0)
		{
			serial->flg.b1 = 1;
		}
	}
}

void USART1_IRQHandler()
{
#ifdef UART_COM1_USED
	UartCom1.irq(&UartCom1);
#endif
}

void hardware_checkover()
{
#ifdef UART_COM1_USED
    UartCom1.checkover(&UartCom1);
#endif // UART_COM1_USED
}

static void NVIC_Configuration(struct serial_ *uart,unsigned char sub_pri)
{
    unsigned char no = uart->param.No % UART_COMNUM;
    struct UartCom *curUsart = &systemUsart[no];

    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the USART1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = curUsart->irq;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = sub_pri;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void hardware_usart_init(void)
{
    unsigned char sub_pri = 0;
#ifdef UART_COM1_USED
	UartCom1.init(&UartCom1);
	NVIC_Configuration(&UartCom1,sub_pri++);
#endif
}




