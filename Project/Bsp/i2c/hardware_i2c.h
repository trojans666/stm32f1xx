#ifndef HARDWARE_I2C_H
#define HARDWARE_I2C_H

#define I2C_IC2_WR          0x0000
#define I2C_I2C_RD          (1u << 0)
#define I2C_I2C_ADDR_10BIT  (1u << 2)   //10bit芯片地址
#define I2C_I2C_NO_START    (1u << 4)
#define I2C_I2C_IGNORE_NACK (1u << 5)
#define I2C_I2C_NO_READ_ACK (1u << 6)

// message
struct i2c_msg
{
    unsigned short addr;
    unsigned short flags;
    unsigned short len;
    unsigned char *buf;
};

typedef enum
{
    I2C_PORT_0,
    I2C_PORT_1,
    I2C_PORT_MAX
}i2c_port_e;



//用gpio模拟
struct i2c_device
{
    i2c_port_e i2c_id;
    void *user_data;

    void (*set_sda)(struct i2c_device *i2c,int state);
    void (*set_scl)(struct i2c_device *i2c,int state);
    int (*get_sda)(struct i2c_device *i2c);
    int (*get_scl)(struct i2c_device *i2c);
    void (*udelay)(unsigned int us);
    void (*mdelay)(unsigned int ms);
    void (*send_start)(struct i2c_device *i2c);
    void (*send_stop)(struct i2c_device *i2c);
    void (*send_restart)(struct i2c_device *i2c);
    int (*send_wait_ack)(struct i2c_device *i2c);
    void (*send_ack)(struct i2c_device *i2c);
    void (*send_no_ack)(struct i2c_device *i2c); //不产生ACK应答


    void (*init)(struct i2c_device *i2c);
    unsigned long (*transfer)(struct i2c_device *i2c,struct i2c_msg msg[],unsigned int num);
    int (*write)(struct i2c_device *i2c,
                 unsigned short addr,
                 unsigned short flag,
                 unsigned char *buf,
                 unsigned int count);
    int (*read)(struct i2c_device *i2c,
                unsigned short addr,
                unsigned short flag,
                unsigned char *buf,
                unsigned int count);
};

#define I2C_PORT1_USED  1

#ifdef I2C_PORT1_USED
    extern struct i2c_device i2c;
#endif

void hardware_i2c_init(void);

#endif // HARDWARE_I2C_H
