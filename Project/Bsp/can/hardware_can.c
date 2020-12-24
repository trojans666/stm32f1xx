#include "stm32f4xx_can.h"
#include "hardware_can.h"

#include <string.h>

/*******************************************************************************
  * @说明：测试队列是否为空
  * @参数：Q: 被测试队列指针
  * @返回：队列状态
*******************************************************************************/
CanQueueStatus_e QUEUE_IsEmpty(Can_Queue_t *Q)
{
	if(Q->front == Q->rear)
		return can_queue_true;

	return can_queue_false;
}

/*******************************************************************************
  * @说明：测试队列是否满
  * @参数：Q: 被测试队列指针
  * @返回：队列状态
*******************************************************************************/
CanQueueStatus_e QUEUE_IsFull(Can_Queue_t *Q)
{
	if((Q->rear + 1) % Q->maxLen == Q->front)
		return can_queue_true;

	return can_queue_false;
}
/*******************************************************************************
  * @说明：向队列插入新元素,数据的插入，也即数据的拷贝过程，按照1字节逐个拷贝
  * @参数：Q: 被操作的队列指针
  *		   e: 新元素
  * @返回：队列状态
*******************************************************************************/
CanQueueStatus_e QUEUE_EnQueue(Can_Queue_t *Q, void *e)
{
	cqu32 size = Q->typeSize;
	char *s  = NULL;
	char *d  = NULL;
	cqu32 front;
	cqu32 rear = Q->rear;

	if(QUEUE_IsFull(Q) == can_queue_true)
		return can_queue_false;

	s  = (char *)e;
	d  = (char *)((cqu32)Q->base + rear * size);

	memcpy(d, s, size);

	s = d = NULL;
	Q->rear = (Q->rear+1) % Q->maxLen;
	//Q->used++;
#if 1
	rear = Q->rear;
	front = Q->front;
	if(rear >= front)
		Q->used = rear - front;
	else
		Q->used = rear + Q->maxLen - front;
#endif
	return can_queue_true;
}
/*******************************************************************************
  * @说明：删除队列头元素,数据的出列，也即数据的拷走过程，按1字节逐个拷走
  * @参数：Q: 被操作的队列指针
  *		   e: 带回头元素
  * @返回：队列状态
*******************************************************************************/
CanQueueStatus_e QUEUE_DeQueue(Can_Queue_t *Q, void *e ,qu32 loc)
{
	cqu32 size = Q->typeSize;
	char *d  = NULL;
	cqu32 pos = loc;
	cqu32 front = (Q->front+pos)%Q->maxLen;
	cqu32 rear;
	char *s  = NULL;
	char *s1 = NULL;

	if(QUEUE_IsEmpty(Q) == can_queue_true)
		return can_queue_false;

	if(pos > Q->used)
		return can_queue_false;

	d = (char *)e;
	s = (char *)Q->base + front * size;
    memcpy(d, s, size);

	while(pos--)
	{
		s = (char *)Q->base + front * size;
		if(front>0)
		{
			front--;
		}
		else
		{
			front = Q->maxLen-1;
		}
		s1 = (char *)Q->base + front * size;


	    memcpy(s, s1, size);
	}
	d = s = NULL;
	Q->front = (Q->front + 1) % Q->maxLen;
	//Q->used--;
	#if 1
	rear = Q->rear;
	front = Q->front;
	if(rear >= front)
		Q->used = rear - front;
	else
		Q->used = rear + Q->maxLen - front;
	#endif
	return can_queue_true;
}

/*******************************************************************************
  * @说明：删除队列
  * @参数：Q: 队列
  * @返回：操作状态
*********************************************************************************/
CanQueueStatus_e QUEUE_Destroy(Can_Queue_t *Q)
{
	//free(Q->base);
	Q->base = NULL;
	Q->front = Q->rear = 0;
	Q->maxLen = Q->used = Q->typeSize = 0;
	return can_queue_true;
}
/*******************************************************************************
  * @说明：清除队列
  * @参数：Q: 队列
  * @返回：操作状态
*********************************************************************************/
CanQueueStatus_e QUEUE_Clear(Can_Queue_t *Q)
{
	memset(Q->base, 0, Q->maxLen * Q->typeSize);
	Q->front = Q->rear = 0;
	Q->used = 0;
	return can_queue_true;
}
/*******************************************************************************
  * @说明：遍历队列
  * @参数：Q: 队列
  * @返回：操作状态
*********************************************************************************/
cqu32 QUEUE_Traverse(Can_Queue_t *Q, void *buf ,qu32 length)
{
	cqu32 used = Q->used;
	cqu32 size = Q->typeSize;
	char *d  = (char *)buf;
	cqu32 front = Q->front;
	char *s  = NULL;
	cqu32 len = length;

	if(used>len)
		used = len;
	len = used;
	while(used--)
	{
		s  = (char *)Q->base + front * size;
	    memcpy(d, s, size);

		d += size;
		front = (front + 1) % Q->maxLen;
	}
	d = s = NULL;
	return len;
}

/*******************************************************************************
  * @说明：取队列中任意一个数据
  * @参数：Q: 队列
  * @返回：操作状态
*******************************************************************************/
CanQueueStatus_e QUEUE_GetIndex(Can_Queue_t *Q, void *e ,qu32 loc)
{
	cqu32 size = Q->typeSize;
	char *d  = NULL;
	cqu32 pos = loc;
	cqu32 front = (Q->front+pos)%Q->maxLen;
	char *s  = NULL;

	if(QUEUE_IsEmpty(Q) == QUEUE_TRUE)
		return can_queue_true;

	if(pos > Q->used)
		return can_queue_false;

	d = (char *)e;
	s = (char *)Q->base + front * size;
	memcpy(d, s, size);
	d = s = NULL;
	return can_queue_true;
}


/************************* CAN *****************************/
void can_init(struct can_device *can);
unsigned int can_read(struct can_device *can,void *recv_buf,unsigned int len);
void can_write(struct can_device *can,void *send_buf,unsigned int len);
void can_set_filter(struct can_device *can,unsigned char filterNo,unsigned int  filterId,unsigned int  filterMaskId, unsigned int  filterFifo);
void can_tx_interrupt(struct can_device *can);
void can_rx_interrupt(struct can_device *can);
void can_check_over(struct can_device *can);


struct CanPort
{
    CAN_TypeDef *CANx;
};
static struct CanPort systemCAN[CAN_PORT_MAX];


#define CAN_PORT1_USED
#ifdef CAN_PORT1_USED

#define CAN_COM1_TX_LEN 50
#define CAN_COM1_RX_LEN 50

unsigned char CanCom1txbuf[CAN_COM1_TX_LEN];
unsigned char CanCom1rxbuf[CAN_COM1_RX_LEN];

Can_Queue_t CanCom1TxList;
Can_Queue_t CanCom1RxList;

struct can_device can1 =
{
    {
      CAN_PORT_1,
      0xFFFF,// canid
      CAN250kBaud,
      CAN_Mode_Normal, //can mode
      //filter
      {
          0,
          0,
          0,
          0
      },
    },
    //flag
    0,
    &CanCom1TxList,
    &CanCom1RxList,
    can_init,
    can_read,
    can_write,
    can_set_filter,
    can_tx_interrupt,
    can_rx_interrupt,
    can_check_over
};

void CAN1_Configuration(can_param_t *param)
{

}

#endif // CAN_PORT1_USED

void can_init(struct can_device *can)
{
    unsigned char no = can->param.no % CAN_PORT_MAX;
    struct CanPort *curCan = &systemCAN[no];
    switch(no)
    {
#ifdef CAN_PORT1_USED
    case CAN_PORT_1:
        curCan->CANx = CAN1;
        CAN_QUEUE_INIT(CanCom1TxList,unsigned char,CanCom1txbuf,CAN_COM1_TX_LEN);
        CAN_QUEUE_INIT(CanCom1RxList,unsigned char,CanCom1rxbuf,CAN_COM1_RX_LEN);
        //can1 config
        CAN1_Configuration(&can->param);
        break;
#endif // CAN_PORT1_USED
    default:
        break;
    }
}

unsigned int can_read(struct can_device *can,void *recv_buf,unsigned int len)
{
    unsigned int n = 0;
    if(NULL == recv_buf)
        return 0;
    //一次只收一个字节
    while((can->RxList->IsEmpty(can->RxList) != can_queue_true) && (len--))
    {
        can->RxList->DeQueue(can->RxList,((unsigned char *)recv_buf + n),0);
        n++;
    }
    return n;
}
void can_write(struct can_device *can,void *send_buf,unsigned int len)
{
    unsigned char no = can->param.no % CAN_PORT_MAX;
    struct CanPort *curCan = &systemCAN[no];

    CanTxMsg *ptr = (CanTxMsg *)send_buf;
    CanTxMsg dat;
    if(NULL == send_buf)
        return ;
    while(len--)
    {
        can->TxList->EnQueue(can->TxList,ptr++);
    }
    if(0 == (can->flag & 0x01))
    {
        can->flag = 1;
        can->TxList->DeQueue(can->TxList,&dat,0);
        CAN_Transmit(curCan->CANx,&dat);
    }
}
void can_set_filter(struct can_device *can,unsigned char filterNo,unsigned int  filterId,unsigned int  filterMaskId, unsigned int  filterFifo)
{
    CAN_FilterInitTypeDef	CAN_FilterInitStructure;

	/* 设置过滤器 */
	CAN_FilterInitStructure.CAN_FilterNumber = filterNo;			  //选择过滤器组1
	CAN_FilterInitStructure.CAN_FilterMode   = CAN_FilterMode_IdMask; //标识符屏蔽位模式
	CAN_FilterInitStructure.CAN_FilterScale  = CAN_FilterScale_32bit; //过滤器位宽32位

	CAN_FilterInitStructure.CAN_FilterIdHigh = ((filterId<<3) >> 16) & 0xffff;
	CAN_FilterInitStructure.CAN_FilterIdLow  = ((filterId<<3) | CAN_ID_EXT | CAN_RTR_DATA) & 0xffff;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (filterMaskId >> 16);
	CAN_FilterInitStructure.CAN_FilterMaskIdLow  = ((filterMaskId << 16) >> 16);

	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = filterFifo; //滤过的报文放到fifo0
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;		   //激活过滤器
	CAN_FilterInit(&CAN_FilterInitStructure);
}
void can_tx_interrupt(struct can_device *can);
void can_rx_interrupt(struct can_device *can);
void can_check_over(struct can_device *can);

void hardware_can_init()
{

}
