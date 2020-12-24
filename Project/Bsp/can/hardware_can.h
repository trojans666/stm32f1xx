#ifndef HARDWARE_CAN_H
#define HARDWARE_CAN_H

/*队列 */
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


/*=============================数据类型定义===================================*/
// 队列状态类型
typedef enum queue_status
{
	can_queue_false=0, can_queue_true
}CanQueueStatus_e;


//队列描述符
typedef struct queue
{
	//队列属性域
	void		*base;		//初始化的动态分配空间
	cqu32		front;		//头指针，若队列不空，则指向队列头元素
	cqu32		rear;		//尾指针，若队列不空，则指向队列尾元素的下一个位置
	cqu32		maxLen;		//队列长度，即单元个数
	cqu32		used;		//队列中含有数据个数
	cqu32		typeSize;	//队列所支持数据类型的大小
	// 队列函数操作域
	CanQueueStatus_e (*EnQueue)(struct queue *Q, void *e);	//插入e为队列的新元素
	CanQueueStatus_e (*DeQueue)(struct queue *Q, void *e, cqu32 loc);	//删除队列头元素
	CanQueueStatus_e (*IsEmpty)(struct queue *Q);			//测试队列是否为空
	CanQueueStatus_e (*IsFull) (struct queue *Q);			//测试队列是否为满
	CanQueueStatus_e (*Destroy)(struct queue *Q);			//销毁队列
	CanQueueStatus_e (*Clear)(struct queue *Q);				//清除队列
	cqu32 (*Traverse)(struct queue *Q, void *buf ,cqu32 length);//遍历队列
	CanQueueStatus_e (*GetIndex)(struct queue *Q, void *e, cqu32 loc);	//获取队列中数据
}Can_Queue_t;



/*=============================函数声明======================================*/

CanQueueStatus_e QUEUE_EnQueue(Can_Queue_t *Q, void *e);	//插入e为队列的新元素

CanQueueStatus_e QUEUE_DeQueue(Can_Queue_t *Q, void *e ,qu32 loc);	//删除队列头元素

CanQueueStatus_e QUEUE_IsEmpty(Can_Queue_t *Q);			//测试队列是否为空

CanQueueStatus_e QUEUE_IsFull(Can_Queue_t *Q);				//测试队列是否为满

CanQueueStatus_e QUEUE_Destroy(Can_Queue_t *Q);			//销毁队列

CanQueueStatus_e QUEUE_Clear(Can_Queue_t *Q);				//清除队列

qu32 QUEUE_Traverse(Can_Queue_t *Q, void *buf ,qu32 length);//遍历队列，只读形式

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

//can过滤器定义
typedef struct
{
    unsigned char filter_no;/*指定要初始化的筛选器 0-13*/
    unsigned int filter_id;/*指定过滤器id号*/
    unsigned int filter_maskid;/*指定过滤掩码号*/
    unsigned int filter_fifo;/*指定将分配给筛选器的FIFO num。CAN_Filter_FIFO0或CAN_Filter_FIFO1*/
}can_filter_t;

typedef struct
{
    unsigned char no;
    unsigned char can_mode;/*can模式 CAN_Mode_Normal or CAN_Mode_LoopBack*/
    unsigned int can_id;/*指定can id。如果不使用指定为0*/
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
