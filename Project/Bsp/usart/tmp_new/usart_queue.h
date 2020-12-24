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
    //����������
	void		*base;		//��ʼ���Ķ�̬����ռ�
	qu32		front;		//ͷָ�룬�����в��գ���ָ�����ͷԪ��
	qu32		rear;		//βָ�룬�����в��գ���ָ�����βԪ�ص���һ��λ��
	qu32		maxLen;		//���г��ȣ�����Ԫ����
	qu32		used;		//�����к������ݸ���
	qu32		typeSize;	//������֧���������͵Ĵ�С
	// ���к���������
	queue_bool_e (*queue_push)(struct queue *Q, void *e);	//����eΪ���е���Ԫ��
	queue_bool_e (*queue_pop)(struct queue *Q, void *e, qu32 loc);	//ɾ������ͷԪ��
	queue_bool_e (*queue_is_empty)(struct queue *Q);			//���Զ����Ƿ�Ϊ��
	queue_bool_e (*queue_is_full) (struct queue *Q);			//���Զ����Ƿ�Ϊ��
	queue_bool_e (*queue_destroy)(struct queue *Q);			//���ٶ���
	queue_bool_e (*queue_clear)(struct queue *Q);				//�������
	qu32 (*queue_traverse)(struct queue *Q, void *buf ,qu32 length);//��������
	queue_bool_e (*queue_get_index)(struct queue *Q, void *e, qu32 loc);	//��ȡ����������
}queue_t;

void queue_init(queue_t *q,qu32 typesize,void *buf,qu32 buf_len);

#endif // QUEUE_H
