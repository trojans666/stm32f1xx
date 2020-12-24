#ifndef HARDWARE_CAN_H
#define HARDWARE_CAN_H

/*���� */
typedef unsigned long cqu32;
#define CAN_QUEUE_INIT(q,type,buf,size)\
		do\
		{	q.front  = q.rear = q.used	= 0;\
			q.typeSize = sizeof(type);\
			q.base = buf;\
			memset(q.base, 0, size*q.typeSize);\
			q.maxLen = size;\
			q.used	= 0;\
			q.EnQueue = QUEUE_EnQueue;\
			q.DeQueue = QUEUE_DeQueue;\
			q.IsEmpty = QUEUE_IsEmpty;\
			q.IsFull  = QUEUE_IsFull;\
			q.Clear   = QUEUE_Clear;\
			q.Traverse= QUEUE_Traverse;\
			q.GetIndex= QUEUE_GetIndex;\
			q.Destroy = QUEUE_Destroy;\
		} while (0);


/*=============================�������Ͷ���===================================*/
// ����״̬����
typedef enum queue_status
{
	can_queue_false=0, can_queue_true
}CanQueueStatus_e;


//����������
typedef struct queue
{
	//����������
	void		*base;		//��ʼ���Ķ�̬����ռ�
	cqu32		front;		//ͷָ�룬�����в��գ���ָ�����ͷԪ��
	cqu32		rear;		//βָ�룬�����в��գ���ָ�����βԪ�ص���һ��λ��
	cqu32		maxLen;		//���г��ȣ�����Ԫ����
	cqu32		used;		//�����к������ݸ���
	cqu32		typeSize;	//������֧���������͵Ĵ�С
	// ���к���������
	CanQueueStatus_e (*EnQueue)(struct queue *Q, void *e);	//����eΪ���е���Ԫ��
	CanQueueStatus_e (*DeQueue)(struct queue *Q, void *e, cqu32 loc);	//ɾ������ͷԪ��
	CanQueueStatus_e (*IsEmpty)(struct queue *Q);			//���Զ����Ƿ�Ϊ��
	CanQueueStatus_e (*IsFull) (struct queue *Q);			//���Զ����Ƿ�Ϊ��
	CanQueueStatus_e (*Destroy)(struct queue *Q);			//���ٶ���
	CanQueueStatus_e (*Clear)(struct queue *Q);				//�������
	cqu32 (*Traverse)(struct queue *Q, void *buf ,cqu32 length);//��������
	CanQueueStatus_e (*GetIndex)(struct queue *Q, void *e, cqu32 loc);	//��ȡ����������
}Can_Queue_t;



/*=============================��������======================================*/

CanQueueStatus_e QUEUE_EnQueue(Can_Queue_t *Q, void *e);	//����eΪ���е���Ԫ��

CanQueueStatus_e QUEUE_DeQueue(Can_Queue_t *Q, void *e ,qu32 loc);	//ɾ������ͷԪ��

CanQueueStatus_e QUEUE_IsEmpty(Can_Queue_t *Q);			//���Զ����Ƿ�Ϊ��

CanQueueStatus_e QUEUE_IsFull(Can_Queue_t *Q);				//���Զ����Ƿ�Ϊ��

CanQueueStatus_e QUEUE_Destroy(Can_Queue_t *Q);			//���ٶ���

CanQueueStatus_e QUEUE_Clear(Can_Queue_t *Q);				//�������

qu32 QUEUE_Traverse(Can_Queue_t *Q, void *buf ,qu32 length);//�������У�ֻ����ʽ

CanQueueStatus_e QUEUE_GetIndex(Sq_Queue *Q, void *e ,qu32 loc);


/************** can **************************/

typedef enum
{
    CAN_PORT_1,
    CAN_PORT_2,
    CAN_PORT_MAX
}can_port_e;

typedef enum
{
    CAN1MBaud   = 1000UL * 1000,/* 1 MBit/sec   */
    CAN800kBaud = 1000UL * 800, /* 800 kBit/sec */
    CAN500kBaud = 1000UL * 500, /* 500 kBit/sec */
    CAN250kBaud = 1000UL * 250, /* 250 kBit/sec */
    CAN125kBaud = 1000UL * 125, /* 125 kBit/sec */
    CAN100kBaud = 1000UL * 100, /* 100 kBit/sec */
    CAN50kBaud  = 1000UL * 50,  /* 50 kBit/sec  */
    CAN20kBaud  = 1000UL * 20,  /* 20 kBit/sec  */
    CAN10kBaud  = 1000UL * 10   /* 10 kBit/sec  */
}can_baudrate_e;

//can����������
typedef struct
{
    unsigned char filter_no;/*ָ��Ҫ��ʼ����ɸѡ�� 0-13*/
    unsigned int filter_id;/*ָ��������id��*/
    unsigned int filter_maskid;/*ָ�����������*/
    unsigned int filter_fifo;/*ָ���������ɸѡ����FIFO num��CAN_Filter_FIFO0��CAN_Filter_FIFO1*/
}can_filter_t;

typedef struct
{
    unsigned char no;
    unsigned char can_mode;/*canģʽ CAN_Mode_Normal or CAN_Mode_LoopBack*/
    unsigned int can_id;/*ָ��can id�������ʹ��ָ��Ϊ0*/
    unsigned int baud_rate;
    can_filter_t filter;
}can_param_t;

struct can_device
{
    can_param_t param;
    unsigned char flag;// bit0 txbusy
    Can_Queue_t *TxList;
    Can_Queue_t *RxList;

    void (*init)(struct can_device *can);
    unsigned int (*read)(struct can_device *can,void *recv_buf,unsigned int len);
    void (*write)(struct can_device *can,void *send_buf,unsigned int len);
    void (*set_filter)(struct can_device *can,unsigned char filterNo,unsigned int  filterId,unsigned int  filterMaskId, unsigned int  filterFifo);
    void (*tx_interrupt)(struct can_device *can);
    void (*rx_interrupt)(struct can_device *can);
    void (*check_over)(struct can_device *can);
};

#define CAN_PORT1_USED  1
#ifdef CAN_PORT1_USED
    extern struct can_device can1;
#endif

void hardware_can_init(void);

#endif // HARDWARE_CAN_H
