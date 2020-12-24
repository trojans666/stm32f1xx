#include "usart_queue.h"

#include <stdio.h>
#include <string.h>

/*******************************************************************************
  * @˵�������Զ����Ƿ�Ϊ��
  * @������Q: �����Զ���ָ��
  * @���أ�����״̬
*******************************************************************************/
queue_bool_e inn_queue_is_empty(queue_t *Q)
{
	if(Q->front == Q->rear)
		return queue_true;

	return queue_false;
}

/*******************************************************************************
  * @˵�������Զ����Ƿ���
  * @������Q: �����Զ���ָ��
  * @���أ�����״̬
*******************************************************************************/
queue_bool_e inn_queue_is_full(queue_t *Q)
{
	if((Q->rear + 1) % Q->maxLen == Q->front)
		return queue_true;

	return queue_false;
}
/*******************************************************************************
  * @˵��������в�����Ԫ��,���ݵĲ��룬Ҳ�����ݵĿ������̣�����1�ֽ��������
  * @������Q: �������Ķ���ָ��
  *		   e: ��Ԫ��
  * @���أ�����״̬
*******************************************************************************/
queue_bool_e inn_queue_push(queue_t *Q, void *e)
{
	qu32 size = Q->typeSize;
	char *s  = NULL;
	char *d  = NULL;
	qu32 front;
	qu32 rear = Q->rear;

	if(inn_queue_is_full(Q) == queue_true)
		return queue_false;

	s  = (char *)e;
	d  = (char *)((qu32)Q->base + rear * size);

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
	return queue_true;
}
/*******************************************************************************
  * @˵����ɾ������ͷԪ��,���ݵĳ��У�Ҳ�����ݵĿ��߹��̣���1�ֽ��������
  * @������Q: �������Ķ���ָ�� loc ���ĸ�λ�ö�
  *		   e: ����ͷԪ��
  * @���أ�����״̬
*******************************************************************************/
queue_bool_e inn_queue_pop(queue_t *Q, void *e ,qu32 loc)
{
	qu32 size = Q->typeSize;
	char *d  = NULL;
	qu32 pos = loc;
	qu32 front = (Q->front+pos)%Q->maxLen;
	qu32 rear;
	char *s  = NULL;
	char *s1 = NULL;

	if(inn_queue_is_empty(Q) == queue_true)
		return queue_false;

	if(pos > Q->used)
		return queue_false;

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
	return queue_true;
}

/*******************************************************************************
  * @˵����ɾ������
  * @������Q: ����
  * @���أ�����״̬
*********************************************************************************/
queue_bool_e inn_queue_destroy(queue_t *Q)
{
//	free(Q->base);
	Q->base = NULL;
	Q->front = Q->rear = 0;
	Q->maxLen = Q->used = Q->typeSize = 0;
	return queue_true;
}
/*******************************************************************************
  * @˵�����������
  * @������Q: ����
  * @���أ�����״̬
*********************************************************************************/
queue_bool_e inn_queue_clear(queue_t *Q)
{
	memset(Q->base, 0, Q->maxLen * Q->typeSize);
	Q->front = Q->rear = 0;
	Q->used = 0;
	return queue_true;
}
/*******************************************************************************
  * @˵������������
  * @������Q: ����
  * @���أ�����״̬
*********************************************************************************/
qu32 inn_queue_traverse(queue_t *Q, void *buf ,qu32 length)
{
	qu32 used = Q->used;
	qu32 size = Q->typeSize;
	char *d  = (char *)buf;
	qu32 front = Q->front;
	char *s  = NULL;
	qu32 len = length;

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
  * @˵����ȡ����������һ������
  * @������Q: ����
  * @���أ�����״̬
*******************************************************************************/
queue_bool_e inn_queue_get_index(queue_t *Q, void *e ,qu32 loc)
{
	qu32 size = Q->typeSize;
	char *d  = NULL;
	qu32 pos = loc;
	qu32 front = (Q->front+pos)%Q->maxLen;
	char *s  = NULL;

	if(inn_queue_is_empty(Q) == queue_true)
		return queue_false;

	if(pos > Q->used)
		return queue_false;

	d = (char *)e;
	s = (char *)Q->base + front * size;
	memcpy(d, s, size);
	d = s = NULL;
	return queue_true;
}



void queue_init(queue_t *q,qu32 typesize,void *buf,qu32 buf_len)
{
    if(q)
    {
        q->front  = q->rear = q->used	= 0;

        q->typeSize = typesize;//sizeof(type);

        q->base = buf;

        memset(q->base, 0, buf_len*q->typeSize);

        q->maxLen = buf_len;

        q->used	= 0;

        q->queue_push = inn_queue_push;

        q->queue_pop = inn_queue_pop;

        q->queue_is_empty = inn_queue_is_empty;

        q->queue_is_full  = inn_queue_is_full;

        q->queue_clear   = inn_queue_clear;

        q->queue_traverse = inn_queue_traverse;

        q->queue_get_index = inn_queue_get_index;

        q->queue_destroy = inn_queue_destroy;
    }
}
