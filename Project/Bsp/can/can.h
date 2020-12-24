#ifndef HARDWARE_CAN_H
#define HARDWARE_CAN_H

void hardware_can_init(void);

unsigned char can1_send_message(unsigned char *msg,unsigned char len);

unsigned char can1_recv_message(unsigned char *msg);

#endif


