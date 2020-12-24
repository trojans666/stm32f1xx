#include "stm32f4xx_can.h"
#include "stm32f4xx_gpio.h"

#include "hardware_can.h"


#define CAN_COM1_BASE								CAN1
#define CAN_COM1_NAME								"CAN1"
#define CAN_COM1_CLOCK							RCC_APB1Periph_CAN1

#define CAN_COM1_TX_PIN							GPIO_Pin_11  //引脚
#define CAN_COM1_TX_GPIO						GPIOA
#define CAN_COM1_TX_GPIO_CLOCK			RCC_AHB1Periph_GPIOA

#define CAN_COM1_TX_GPIO_SOURCE			GPIO_PinSource11			//复用
#define CAN_COM1_TX_AF							GPIOA				//复用引脚的组


#define CAN_COM1_RX_PIN							GPIO_Pin_12
#define CAN_COM1_RX_GPIO						GPIOA
#define CAN_COM1_RX_GPIO_CLOCK			RCC_AHB1Periph_GPIOA

#define CAN_COM1_RX_GPIO_SOURCE			GPIO_PinSource12
#define CAN_COM1_RX_AF							GPIOA

#define CAN_COM1_GPIO_AF						GPIO_AF_CAN1

void hardware_can_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	CAN_InitTypeDef CAN_InitStruct;
	CAN_FilterInitTypeDef CAN_FilterInitStruct;
	
	/* 时钟*/
	RCC_AHB1PeriphClockCmd(CAN_COM1_TX_GPIO_CLOCK | CAN_COM1_RX_GPIO_CLOCK,ENABLE);
	RCC_APB1PeriphClockCmd(CAN_COM1_CLOCK,ENABLE);
	
	/* gpio初始化*/
	GPIO_InitStruct.GPIO_Pin = CAN_COM1_TX_PIN | CAN_COM1_RX_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	/* 引脚复用 */
	GPIO_PinAFConfig(CAN_COM1_TX_AF,CAN_COM1_TX_GPIO_SOURCE,CAN_COM1_GPIO_AF);
	GPIO_PinAFConfig(CAN_COM1_RX_AF,CAN_COM1_RX_GPIO_SOURCE,CAN_COM1_GPIO_AF);
	
	//CAN单元设置
   	CAN_InitStruct.CAN_TTCM=DISABLE;	//非时间触发通信模式   
  	CAN_InitStruct.CAN_ABOM=DISABLE;	//软件自动离线管理	  
  	CAN_InitStruct.CAN_AWUM=DISABLE;//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
  	CAN_InitStruct.CAN_NART=ENABLE;	//禁止报文自动传送 
  	CAN_InitStruct.CAN_RFLM=DISABLE;	//报文不锁定,新的覆盖旧的  
  	CAN_InitStruct.CAN_TXFP=DISABLE;	//优先级由报文标识符决定 
  	CAN_InitStruct.CAN_Mode= CAN_Mode_Normal;//CAN_Mode_LoopBack;	 //模式设置 
  	CAN_InitStruct.CAN_SJW=CAN_SJW_1tq;	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStruct.CAN_BS1=CAN_BS1_6tq; //Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStruct.CAN_BS2=CAN_BS2_7tq;//Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStruct.CAN_Prescaler=6;  //分频系数(Fdiv)为brp+1	
  	CAN_Init(CAN1, &CAN_InitStruct);   // 初始化CAN1 
		
		//配置过滤器
 	  CAN_FilterInitStruct.CAN_FilterNumber=0;	  //过滤器0
  	CAN_FilterInitStruct.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStruct.CAN_FilterScale=CAN_FilterScale_32bit; //32位 
  	CAN_FilterInitStruct.CAN_FilterIdHigh=0x0000;////32位ID
  	CAN_FilterInitStruct.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStruct.CAN_FilterMaskIdHigh=0x0000;//32位MASK
  	CAN_FilterInitStruct.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStruct.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
  	CAN_FilterInitStruct.CAN_FilterActivation=ENABLE; //激活过滤器0
  	CAN_FilterInit(&CAN_FilterInitStruct);//滤波器初始化
}

u8 can1_send_message(u8 *msg,u8 len)
{
	u8 mbox;
	u16 i = 0;
	
	CanTxMsg TxMessage;
	TxMessage.StdId = 0x12;
	TxMessage.ExtId = 0x12;
	TxMessage.IDE = 0;
	TxMessage.RTR = 0;
	TxMessage.DLC = (len <= 8) ? len : 8;
	
	for(i = 0;i < TxMessage.DLC;i++)
	{
		TxMessage.Data[i] = msg[i];
	}
	
	mbox = CAN_Transmit(CAN1,&TxMessage);
	
	i = 0;
	while((CAN_TransmitStatus(CAN1,mbox) == CAN_TxStatus_Failed) && (i < 0xFFF))
	{
		i++;
	}
	
	if(i >= 0xFFF)
		return 1;
	
	return 0;
}
	
u8 can1_recv_message(u8 *msg)
{
	u32 i;
	CanRxMsg RxMessage;
	if(CAN_MessagePending(CAN1,CAN_FIFO0) == 0)
	{
		return 0;
	}
	
	CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
	
	for(i = 0;i < RxMessage.DLC;i++)
	{
		msg[i] = RxMessage.Data[i];
	}
	
	return RxMessage.DLC;
}

