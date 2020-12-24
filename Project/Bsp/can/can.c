#include "stm32f4xx_can.h"
#include "stm32f4xx_gpio.h"

#include "hardware_can.h"


#define CAN_COM1_BASE								CAN1
#define CAN_COM1_NAME								"CAN1"
#define CAN_COM1_CLOCK							RCC_APB1Periph_CAN1

#define CAN_COM1_TX_PIN							GPIO_Pin_11  //����
#define CAN_COM1_TX_GPIO						GPIOA
#define CAN_COM1_TX_GPIO_CLOCK			RCC_AHB1Periph_GPIOA

#define CAN_COM1_TX_GPIO_SOURCE			GPIO_PinSource11			//����
#define CAN_COM1_TX_AF							GPIOA				//�������ŵ���


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
	
	/* ʱ��*/
	RCC_AHB1PeriphClockCmd(CAN_COM1_TX_GPIO_CLOCK | CAN_COM1_RX_GPIO_CLOCK,ENABLE);
	RCC_APB1PeriphClockCmd(CAN_COM1_CLOCK,ENABLE);
	
	/* gpio��ʼ��*/
	GPIO_InitStruct.GPIO_Pin = CAN_COM1_TX_PIN | CAN_COM1_RX_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	/* ���Ÿ��� */
	GPIO_PinAFConfig(CAN_COM1_TX_AF,CAN_COM1_TX_GPIO_SOURCE,CAN_COM1_GPIO_AF);
	GPIO_PinAFConfig(CAN_COM1_RX_AF,CAN_COM1_RX_GPIO_SOURCE,CAN_COM1_GPIO_AF);
	
	//CAN��Ԫ����
   	CAN_InitStruct.CAN_TTCM=DISABLE;	//��ʱ�䴥��ͨ��ģʽ   
  	CAN_InitStruct.CAN_ABOM=DISABLE;	//����Զ����߹���	  
  	CAN_InitStruct.CAN_AWUM=DISABLE;//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
  	CAN_InitStruct.CAN_NART=ENABLE;	//��ֹ�����Զ����� 
  	CAN_InitStruct.CAN_RFLM=DISABLE;	//���Ĳ�����,�µĸ��Ǿɵ�  
  	CAN_InitStruct.CAN_TXFP=DISABLE;	//���ȼ��ɱ��ı�ʶ������ 
  	CAN_InitStruct.CAN_Mode= CAN_Mode_Normal;//CAN_Mode_LoopBack;	 //ģʽ���� 
  	CAN_InitStruct.CAN_SJW=CAN_SJW_1tq;	//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStruct.CAN_BS1=CAN_BS1_6tq; //Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStruct.CAN_BS2=CAN_BS2_7tq;//Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStruct.CAN_Prescaler=6;  //��Ƶϵ��(Fdiv)Ϊbrp+1	
  	CAN_Init(CAN1, &CAN_InitStruct);   // ��ʼ��CAN1 
		
		//���ù�����
 	  CAN_FilterInitStruct.CAN_FilterNumber=0;	  //������0
  	CAN_FilterInitStruct.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStruct.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
  	CAN_FilterInitStruct.CAN_FilterIdHigh=0x0000;////32λID
  	CAN_FilterInitStruct.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStruct.CAN_FilterMaskIdHigh=0x0000;//32λMASK
  	CAN_FilterInitStruct.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStruct.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
  	CAN_FilterInitStruct.CAN_FilterActivation=ENABLE; //���������0
  	CAN_FilterInit(&CAN_FilterInitStruct);//�˲�����ʼ��
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

