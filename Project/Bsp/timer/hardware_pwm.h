#ifndef HARDWARE_PWM_H
#define HARDWARE_PWM_H

typedef enum
{
    PWM_PORT_0,
    PWM_PORT_MAX
}pwm_port_e;

struct pwm_config
{
    unsigned int period;//pwm输出周期
    unsigned int duty_cycle;//占空比
    unsigned int polarity;//极性 0表示高电平还是1表示高电平
   // unsigned int enable;//使能
};

struct pwm_device
{
    pwm_port_e pwm_id;
    struct pwm_config config;

    void (*init)(struct pwm_device *pwm);
    void (*enable)(struct pwm_device *pwm);
    void (*configure)(struct pwm_device *pwm,struct pwm_config *cfg);
    //修改极性
    void (*set_polarity)(struct pwm_device *pwm,unsigned char polarity);
    //修改占空比
    void (*set_compare)(struct pwm_device *pwm,unsigned int value);
};

#define PWM_PORT0_USED	1
#ifdef PWM_PORT0_USED
    extern struct pwm_device pwm14;
#endif

void hardware_pwm_init(void);



#endif // HARDWARE_PWM_H
