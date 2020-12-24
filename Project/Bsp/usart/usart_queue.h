#ifndef USART_QUEUE_H
#define USART_QUEUE_H

typedef unsigned long qu32;
typedef enum
{
    queue_false = 0,
    queue_true = !queue_false
}queue_bool_e;

typedef struct queue
{
    //队列属性域
	void		*base;		//初始化的动态分配空间
	qu32		front;		//头指针，若队列不空，则指向队列头元素
	qu32		rear;		//尾指针，若队列不空，则指向队列尾元素的下一个位置
	qu32		maxLen;		//队列长度，即单元个数
	qu32		used;		//队列中含有数据个数
	qu32		typeSize;	//队列所支持数据类型的大小
	// 队列函数操作域
	queue_bool_e (*queue_push)(struct queue *Q, void *e);	//插入e为队列的新元素
	queue_bool_e (*queue_pop)(struct queue *Q, void *e, qu32 loc);	//删除队列头元素
	queue_bool_e (*queue_is_empty)(struct queue *Q);			//测试队列是否为空
	queue_bool_e (*queue_is_full) (struct queue *Q);			//测试队列是否为满
	queue_bool_e (*queue_destroy)(struct queue *Q);			//销毁队列
	queue_bool_e (*queue_clear)(struct queue *Q);				//清除队列
	qu32 (*queue_traverse)(struct queue *Q, void *buf ,qu32 length);//遍历队列
	queue_bool_e (*queue_get_index)(struct queue *Q, void *e, qu32 loc);	//获取队列中数据
}queue_t;

void queue_init(queue_t *q,qu32 typesize,void *buf,qu32 buf_len);

#endif // QUEUE_H
