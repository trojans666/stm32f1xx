#ifndef HARDWARE_PWM_H
#define HARDWARE_PWM_H

typedef enum
{
    PWM_PORT_0,
    PWM_PORT_MAX
}pwm_port_e;

struct pwm_config
{
    unsigned int period;//pwm�������
    unsigned int duty_cycle;//ռ�ձ�
    unsigned int polarity;//���� 0��ʾ�ߵ�ƽ����1��ʾ�ߵ�ƽ
   // unsigned int enable;//ʹ��
};

struct pwm_device
{
    pwm_port_e pwm_id;
    struct pwm_config config;

    void (*init)(struct pwm_device *pwm);
    void (*enable)(struct pwm_device *pwm);
    void (*configure)(struct pwm_device *pwm,struct pwm_config *cfg);
    //�޸ļ���
    void (*set_polarity)(struct pwm_device *pwm,unsigned char polarity);
    //�޸�ռ�ձ�
    void (*set_compare)(struct pwm_device *pwm,unsigned int value);
};

#define PWM_PORT0_USED	1
#ifdef PWM_PORT0_USED
    extern struct pwm_device pwm14;
#endif

void hardware_pwm_init(void);



#endif // HARDWARE_PWM_H
