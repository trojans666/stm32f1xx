#include "stm32f4xx_gpio.h"
#include "hardware_systick.h"
#include "hardware_i2c.h"

void i2c_set_sda(struct i2c_device *i2c,int state);
void i2c_set_scl(struct i2c_device *i2c,int state);
int i2c_get_sda(struct i2c_device *i2c);
int i2c_get_scl(struct i2c_device *i2c);
void i2c_udelay(unsigned int us);
void i2c_mdelay(unsigned int ms);
void i2c_send_start(struct i2c_device *i2c);
void i2c_send_stop(struct i2c_device *i2c);
void i2c_send_restart(struct i2c_device *i2c);
int i2c_send_wait_ack(struct i2c_device *i2c);
void i2c_send_ack(struct i2c_device *i2c);
void i2c_send_no_ack(struct i2c_device *i2c); //不产生ACK应答

void i2c_init(struct i2c_device *i2c);
unsigned long i2c_transfer(struct i2c_device *i2c,struct i2c_msg msg[],unsigned int num);
int i2c_write(struct i2c_device *i2c,
                 unsigned short addr,
                 unsigned short flag,
                 unsigned char *buf,
                 unsigned int count);
int i2c_read(struct i2c_device *i2c,
                unsigned short addr,
                unsigned short flag,
                unsigned char *buf,
                unsigned int count);

typedef void (*i2c_gpio_rcc_cb)(unsigned int,FunctionalState);
struct i2c_gpio_pin
{
    i2c_gpio_rcc_cb rcc_ptr;
    unsigned int rcc;
    GPIO_TypeDef *gpio;
    unsigned int pin;
};
#define I2C_GPIO_PIN_CONFIG(rcc,gpio,gpio_index) { RCC_##rcc##PeriphClockCmd,RCC_##rcc##Periph_GPIO##gpio, GPIO##gpio, GPIO_Pin_##gpio_index}

struct i2c_pin
{
    struct i2c_gpio_pin i2c_scl;
    struct i2c_gpio_pin i2c_sda;
};

#ifdef I2C_PORT1_USED

struct i2c_pin i2c_gpios =
{
    I2C_GPIO_PIN_CONFIG(AHB1,B,8), //SCL
    I2C_GPIO_PIN_CONFIG(AHB1,B,9) //SDA
};

struct i2c_device i2c =
{
    I2C_PORT_0,
    &i2c_gpios,
    i2c_set_sda,
    i2c_set_scl,
    i2c_get_sda,
    i2c_get_scl,
    i2c_udelay,
    i2c_mdelay,
    i2c_send_start,
    i2c_send_stop,
    i2c_send_restart,
    i2c_send_wait_ack,
    i2c_send_ack,
    i2c_send_no_ack,
    i2c_init,
    i2c_transfer,
    i2c_write,
    i2c_read
};

static void I2c_Configurate()
{
    GPIO_InitTypeDef GPIO_InitStruct;

    //scl init
    i2c_gpios.i2c_scl.rcc_ptr(i2c_gpios.i2c_scl.rcc,ENABLE);
    GPIO_InitStruct.GPIO_Pin = i2c_gpios.i2c_scl.pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;//普通输出
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;//上拉

    GPIO_Init(i2c_gpios.i2c_scl.gpio,&GPIO_InitStruct);

    //sda init
    i2c_gpios.i2c_sda.rcc_ptr(i2c_gpios.i2c_sda.rcc,ENABLE);
    GPIO_InitStruct.GPIO_Pin = i2c_gpios.i2c_sda.pin;
    GPIO_Init(i2c_gpios.i2c_sda.gpio,&GPIO_InitStruct);
}

#endif // I2C_PORT1_USED

void i2c_set_sda(struct i2c_device *i2c,int state)
{
    struct i2c_pin *gpio = i2c->user_data;
    if(gpio)
    {
        if(state == 1)
        {
            GPIO_SetBits(gpio->i2c_sda.gpio,gpio->i2c_sda.pin);
        }
        else if(state == 0)
        {
            GPIO_ResetBits(gpio->i2c_sda.gpio,gpio->i2c_sda.pin);
        }
    }
}

void i2c_set_scl(struct i2c_device *i2c,int state)
{
    struct i2c_pin *gpio = i2c->user_data;
    if(gpio)
    {
        if(state == 1)
        {
            GPIO_SetBits(gpio->i2c_scl.gpio,gpio->i2c_scl.pin);
        }
        else if(state == 0)
        {
            GPIO_ResetBits(gpio->i2c_scl.gpio,gpio->i2c_scl.pin);
        }
    }
}

int i2c_get_sda(struct i2c_device *i2c)
{
    struct i2c_pin *gpio = i2c->user_data;

    return (int)GPIO_ReadInputDataBit(gpio->i2c_sda.gpio,gpio->i2c_sda.pin);
}

int i2c_get_scl(struct i2c_device *i2c)
{
    struct i2c_pin *gpio = i2c->user_data;

    return (int)GPIO_ReadInputDataBit(gpio->i2c_scl.gpio,gpio->i2c_scl.pin);
}

#define SDA_H(i2c)  i2c_set_sda(i2c,1)
#define SDA_L(i2c)  i2c_set_sda(i2c,0)

#define SCL_H(i2c)  i2c_set_scl(i2c,1)
#define SCL_L(i2c)  i2c_set_scl(i2c,0)

#define GET_SDA(i2c)    i2c_get_sda(i2c)
#define GET_SCL(i2c)    i2c_get_scl(i2c)

void i2c_send_start(struct i2c_device *i2c)
{
    SDA_H(i2c);
    SCL_H(i2c);
    i2c_udelay(4);
    SDA_L(i2c);
    i2c_udelay(4);
    SCL_L(i2c);//钳住i2c总线。准备发送数据
}
void i2c_send_stop(struct i2c_device *i2c)
{
    SCL_L(i2c);
    SDA_L(i2c);
    i2c_udelay(4);
    SCL_H(i2c);
    SDA_H(i2c);
    i2c_udelay(4);
}
void i2c_send_restart(struct i2c_device *i2c)
{
    SDA_H(i2c);
    SCL_H(i2c);
    i2c_udelay(4);
    SDA_L(i2c);
    i2c_udelay(4);
    SCL_L(i2c);
}
int i2c_send_wait_ack(struct i2c_device *i2c)
{
    unsigned char ErrTime = 0;
    SCL_H(i2c);
    SCL_L(i2c);
    while(GET_SDA(i2c))
    {
        ErrTime++;
        if(ErrTime > 250)
        {
            i2c_send_stop(i2c);
            return 0;
        }
    }
    return 1;
}
//产生ack
void i2c_send_ack(struct i2c_device *i2c)
{
    SCL_L(i2c);
    SDA_L(i2c);
    i2c_udelay(2);
    SCL_H(i2c);
    i2c_udelay(2);
    SCL_L(i2c);
}
void i2c_send_no_ack(struct i2c_device *i2c) //不产生ACK应答
{
    SCL_L(i2c);
    SDA_H(i2c);
    i2c_udelay(2);
    SCL_H(i2c);
    i2c_udelay(2);
    SCL_L(i2c);
}

static int i2c_writeb(struct i2c_device *i2c,unsigned char data)
{
    int i = 0;
    unsigned char bit;
    for(i = 7; i>= 0; i--)
    {
        SCL_L(i2c);
        bit = (data >> i) & 1;
			  i2c_set_sda(i2c,bit);
        i2c_udelay(2);
        SCL_H(i2c);
    }

    SCL_L(i2c);
    i2c_udelay(2);

    return i2c_send_wait_ack(i2c);
}

static int i2c_readb(struct i2c_device *i2c)
{
    unsigned char i = 0;
    unsigned char data = 0;

    SDA_H(i2c);
    i2c_udelay(2);
    for(i = 0; i < 8; i++)
    {
        data <<= 1;
        SCL_H(i2c);
        if(GET_SDA(i2c))
        {
            data |= 1;
        }
        SCL_L(i2c);
        i2c_udelay(2);
    }
    return data;
}

static int i2c_send_bytes(struct i2c_device *i2c,struct i2c_msg *msg)
{
    int ret;
    int bytes = 0;
    unsigned char *ptr = msg->buf;
    int count = msg->len;
    unsigned short ignore_nack = msg->flags & I2C_I2C_IGNORE_NACK;

    while(count > 0)
    {
        ret = i2c_writeb(i2c,*ptr);
        if((ret > 0) || (ignore_nack && (ret == 0)))
        {
            count--;
            ptr++;
            bytes++;
        }
        else if(ret == 0)
        {
            return 0;
        }
        else
        {
            return ret;
        }
    }

    return bytes;
}

static int i2c_send_ack_or_nack(struct i2c_device *i2c,int ack)
{
    if(ack)
    {
        SDA_L(i2c);
    }
    i2c_udelay(2);
    SCL_H(i2c);
    SCL_L(i2c);

    return 1;
}

static int i2c_recv_bytes(struct i2c_device *i2c,struct i2c_msg *msg)
{
    int val;
    int bytes = 0;
    unsigned char *ptr = msg->buf;
    int count = msg->len;
    int flags = msg->flags;

    while(count > 0)
    {
        val = i2c_readb(i2c);
        if(val >= 0)
        {
            *ptr = val;
            bytes++;
        }
        else
        {
            break;
        }
        ptr++;
        count--;

        if(!(flags & I2C_I2C_NO_READ_ACK))
        {
            val = i2c_send_ack_or_nack(i2c,count);
            if(val < 0)
                return val;
        }
    }

    return bytes;
}

static int i2c_send_address(struct i2c_device *i2c,unsigned char addr,int retries)
{
    int i;
    int ret = 0;

    for(i = 0; i <= retries; i++)
    {
        ret = i2c_writeb(i2c,addr);
        if(ret == 1 || i == retries)
        {
            break;
        }
        i2c_send_stop(i2c);
        i2c_udelay(2);
        i2c_send_start(i2c);
    }

    return ret;
}

static int i2c_bit_send_address(struct i2c_device *i2c,struct i2c_msg *msg)
{
    unsigned short flags = msg->flags;
    unsigned short ignore_nack = msg->flags & I2C_I2C_IGNORE_NACK;

    unsigned char addr1,addr2;
    int retries;
    int ret;

    retries = ignore_nack ? 0 : 1;

    if(flags & I2C_I2C_ADDR_10BIT)
    {
        addr1 = 0xf0 | ((msg->addr >> 7) & 0x06);
        addr2 = msg->addr & 0xff;

        ret = i2c_send_address(i2c,addr1,retries);
        if((ret != 1) && !ignore_nack)
        {
            return -1;
        }

        ret = i2c_writeb(i2c,addr2);
        if((ret != 1) && !ignore_nack)
        {
            return -1;
        }
        if(flags & I2C_I2C_RD)
        {
            i2c_send_restart(i2c);
            addr1 |= 0x01;
            ret = i2c_send_address(i2c,addr1,retries);
            if((ret != 1) && !ignore_nack)
            {
                return -1;
            }
        }
    }
    else
    {
        /* 7 bit addr */
        addr1 = msg->addr << 1;
        if(flags & I2C_I2C_RD)
        {
            addr1 |= 1;
        }
        ret = i2c_send_address(i2c,addr1,retries);
        if((ret != 1) && !ignore_nack)
        {
            return -1;
        }
    }

    return 1;
}

void i2c_udelay(unsigned int us)
{
    systick_delay_us(us);
}

void i2c_mdelay(unsigned int ms)
{
    i2c_udelay(ms * 1000);
}

void i2c_init(struct i2c_device *i2c)
{
    unsigned char no = i2c->i2c_id % I2C_PORT_MAX;

    switch(no)
    {
#ifdef I2C_PORT1_USED
    case I2C_PORT_0:
        I2c_Configurate();
        break;
#endif // I2C_PORT1_USED
    default:
        break;
    }
}


unsigned long i2c_transfer(struct i2c_device *i2c,struct i2c_msg msgs[],unsigned int num)
{
    struct i2c_msg *msg;
    int i = 0;
    int ret = -1;
    unsigned short ignore_nack;

    i2c_send_start(i2c); //起始信号
    for(i = 0; i < num; i++)
    {
        msg = &msgs[i];
        ignore_nack = msg->flags & I2C_I2C_IGNORE_NACK;
        if(!(msg->flags & I2C_I2C_NO_START))
        {
            if(i)
            {
                i2c_send_restart(i2c);//每次都要重新发起始信号
            }
            ret = i2c_bit_send_address(i2c,msg);
            if((ret != 1) && !ignore_nack)
            {
                i2c_send_stop(i2c);
                return ret;
            }
        }
        if(msg->flags & I2C_I2C_RD)
        {
            ret = i2c_recv_bytes(i2c,msg);
            if(ret < msg->len)
            {
                if(ret >= 0)
                {
                    i2c_send_stop(i2c);
                    return 0;
                }
            }

        }
        else
        {
            ret = i2c_send_bytes(i2c,msg);
            if(ret < msg->len)
            {
                if(ret >= 0)
                {
                    i2c_send_stop(i2c);
                    return 0;
                }
            }
        }
    }
    ret = i;

    return ret;
}

int i2c_write(struct i2c_device *i2c,
                 unsigned short addr,
                 unsigned short flag,
                 unsigned char *buf,
                 unsigned int count)
{
    int ret;
    struct i2c_msg msg;
    msg.addr = addr;
    msg.flags = flag & I2C_I2C_ADDR_10BIT;
    msg.len = count;
    msg.buf = (unsigned char *)buf;

    ret = i2c_transfer(i2c,&msg,1);
    return (ret > 0) ? count : ret;
}

int i2c_read(struct i2c_device *i2c,
                unsigned short addr,
                unsigned short flag,
                unsigned char *buf,
                unsigned int count)
{
    int ret;
    struct i2c_msg msg;

    msg.addr = addr;
    msg.flags = flag & I2C_I2C_ADDR_10BIT;
    msg.flags |= I2C_I2C_RD;
    msg.len = count;
    msg.buf = buf;

    ret = i2c_transfer(i2c,&msg,1);
    return (ret > 0) ? count : ret;
}


void hardware_i2c_init()
{
#ifdef I2C_PORT1_USED
    i2c.init(&i2c);
#endif // I2C_PORT1_USED
}
