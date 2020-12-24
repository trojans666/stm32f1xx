#ifndef HARDWARE_TIMER_H
#define HARDWARE_TIMER_H

typedef void (*handle_timer_cb)(void);

typedef enum
{
    TIMER_PORT_2,
    TIMER_PORT_MAX
}timer_port_e;

struct timer_config
{
    unsigned int ms_reload;
};

typedef struct timer_
{
    unsigned int timer_id;
    handle_timer_cb cb_ptr;
    struct timer_config config;

    void (*timer_init)(struct timer_ *timer);
    void (*timer_reinit)(struct timer_ *timer);
    void (*timer_register_callback)(struct timer_ *timer,handle_timer_cb func);
    void (*timer_unregister_callback)(struct timer_ *timer);
    void (*timer_start)(struct timer_ *timer);
    void (*timer_stop)(struct timer_ *timer);
    void (*timer_irq)(struct timer_ *timer);
    void (*timer_config)(struct timer_ *timer,struct timer_config *config);
}timer_t;

#define TIMER2_USED 1

#ifdef TIMER2_USED
    extern timer_t timer2;
#endif


void hardware_timer_init(void);


#endif // HARDWARE_TIMER_H
