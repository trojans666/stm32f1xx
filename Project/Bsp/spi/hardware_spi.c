#include "stm32f4xx_spi.h"

#include "hardware_spi.h"

#include <string.h>

void spi_init(struct spi_device *spi_dev);
void spi_config(struct spi_device *spi_dev,struct spi_config *config);
unsigned long spi_xfer(struct spi_device *spi_dev,struct spi_message *msg);
int spi_write(struct spi_device *spi_dev,void *data,int len);
int spi_read(struct spi_device *spi_dev,void *data,int len);
int spi_transfer(struct spi_device *spi_dev,void *send_buf,void *recv_buf,int len);
int spi_rdwr_byte(struct spi_device *spi_dev,int data);

struct SpiPort
{
    SPI_TypeDef *SPIx;
};
static struct SpiPort systemSpi[SPI_PORT_MAX];

struct SpiCs
{
    GPIO_TypeDef *GPIOx;
    unsigned short GPIO_Pin;
};

#ifdef SPI_PORT1_USED

#define SPI1_CLOCK					RCC_APB2Periph_SPI1

#define SPI1_SCK_GPIO_CLOCK				RCC_AHB1Periph_GPIOB
#define SPI1_MISO_GPIO_CLOCK			RCC_AHB1Periph_GPIOB
#define SPI1_MOSI_GPIO_CLOCK			RCC_AHB1Periph_GPIOB

#define SPI1_SCK_GPIO_SOURCE			GPIO_PinSource3
#define SPI1_MISO_GPIO_SOURCE			GPIO_PinSource4
#define SPI1_MOSI_GPIO_SOURCE			GPIO_PinSource5

#define SPI1_SCK_PIN							GPIO_Pin_3
#define SPI1_MISO_PIN							GPIO_Pin_4
#define SPI1_MOSI_PIN							GPIO_Pin_5

#define SPI1_SCK_AF_GPIO					GPIOB
#define SPI1_MISO_AF_GPIO					GPIOB
#define SPI1_MOSI_AF_GPIO					GPIOB

#define SPI1_AF_GPIO_SPI					GPIO_AF_SPI1

#if 0
struct SpiCs spi_cs =
{
    GPIOA,
    GPIO_Pin_0
};
#endif

struct spi_device spi1 =
{
    SPI_PORT_1,
    {
        SPI_SPI_MODE_3 | SPI_SPI_MSB,
        8,
        0
    },
    //&spi_cs,
    (0),
    spi_init,
    spi_config,
    spi_xfer,
    spi_write,
    spi_read,
    spi_transfer,
    spi_rdwr_byte
};

#if 0

void unsigned short get_spi_BaudRatePrescaler(unsigned int max_hz)
{
    unsigned short SPI_BaudRatePrescaler;

    /* STM32F10x SPI MAX 18Mhz */
    if(max_hz >= SystemCoreClock/2 && SystemCoreClock/2 <= 36000000)
    {
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    }
    else if(max_hz >= SystemCoreClock/4)
    {
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    }
    else if(max_hz >= SystemCoreClock/8)
    {
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    }
    else if(max_hz >= SystemCoreClock/16)
    {
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    }
    else if(max_hz >= SystemCoreClock/32)
    {
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
    }
    else if(max_hz >= SystemCoreClock/64)
    {
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
    }
    else if(max_hz >= SystemCoreClock/128)
    {
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
    }
    else
    {
        /* min prescaler 256 */
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    }

    return SPI_BaudRatePrescaler;
}

#endif

static void SPI1_Configurate(struct spi_config *config)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    /*使能gpio时钟*/
    RCC_AHB1PeriphClockCmd(SPI1_SCK_GPIO_CLOCK | SPI1_MISO_GPIO_CLOCK | SPI1_MOSI_GPIO_CLOCK, ENABLE);


    /*复用*/
    GPIO_PinAFConfig(SPI1_SCK_AF_GPIO,SPI1_SCK_GPIO_SOURCE,SPI1_AF_GPIO_SPI);
    GPIO_PinAFConfig(SPI1_MISO_AF_GPIO,SPI1_MISO_GPIO_SOURCE,SPI1_AF_GPIO_SPI);
    GPIO_PinAFConfig(SPI1_MOSI_AF_GPIO,SPI1_MOSI_GPIO_SOURCE,SPI1_AF_GPIO_SPI);

    /*使能spi1时钟*/
    RCC_APB2PeriphClockCmd(SPI1_CLOCK, ENABLE);

    GPIO_InitStructure.GPIO_Pin = SPI1_SCK_PIN|SPI1_MISO_PIN|SPI1_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);


    RCC_APB2PeriphResetCmd(SPI1_CLOCK,ENABLE);
    RCC_APB2PeriphResetCmd(SPI1_CLOCK,DISABLE);

    //模式配置

    if(config->data_width <= 8)
    {
        SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//8bit
    }
    else if(config->data_width <= 16)
    {
        SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;		//16bit
    }
    //相位极性
    if(config->mode & SPI_SPI_CPOL)
    {
        SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//空闲状态为高电平
    }
    else
    {
        SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//空闲状态为低电平
    }

    if(config->mode & SPI_SPI_CPHA)
    {
        SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//第二跳变沿采样
    }
    else
    {
        SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//第1跳变沿采样
    }

    if(config->mode & SPI_SPI_MSB)
    {
        SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    }
    else
    {
        SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
    }

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//主模式
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; //模式可配置

    SPI_InitStructure.SPI_CRCPolynomial = 7;
    //SPI_CalculateCRC(SPI1,DISABLE);//禁止多项式
    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_Cmd(SPI1, ENABLE); //开启SPI
}

#endif // SPI_PORT1_USED


void spi_init(struct spi_device *spi_dev)
{
    unsigned char no = spi_dev->spi_id % SPI_PORT_MAX;
    struct SpiPort *curSpi = &systemSpi[no];

    switch(no)
    {
#ifdef SPI_PORT1_USED
    case SPI_PORT_1:
        curSpi->SPIx = SPI1;
        SPI1_Configurate(&spi_dev->config);
        break;
#endif // SPI_PORT1_USED
#ifdef SPI_PORT2_USED
    case SPI_PORT_2:
        break;
#endif // SPI_PORT1_USED
    default:
        break;
    }
}

void spi_config(struct spi_device *spi_dev,struct spi_config *config)
{
    unsigned char no = spi_dev->spi_id % SPI_PORT_MAX;
    switch(no)
    {
#ifdef SPI_PORT1_USED
    case SPI_PORT_1:
        memcpy(&spi_dev->config,config,sizeof(spi_dev->config));
        SPI1_Configurate(config);
        break;
#endif // SPI_PORT1_USED
    default:
        break;
    }
}

unsigned long spi_xfer(struct spi_device *spi_dev,struct spi_message *msg)
{
    struct spi_config *config = &spi_dev->config;
    struct SpiCs *spi_cs = spi_dev->user_data;

    unsigned char no = spi_dev->spi_id % SPI_PORT_MAX;
    struct SpiPort *curSpi = &systemSpi[no];
    unsigned int size = msg->length;

    //take cs
    if(msg->cs_take && spi_cs)
    {
        GPIO_ResetBits(spi_cs->GPIOx,spi_cs->GPIO_Pin);
    }

    if(config->data_width == 8)
    {
        const unsigned char *send_ptr = msg->send_buf;
        unsigned char *recv_ptr = msg->recv_buf;
        unsigned char data = 0;

        while(size--)
        {
            if(send_ptr != (0))
            {
                data = *send_ptr++;
            }

            while (SPI_I2S_GetFlagStatus(curSpi->SPIx, SPI_I2S_FLAG_TXE) == RESET) {}

            SPI_I2S_SendData(curSpi->SPIx, data);

            while (SPI_I2S_GetFlagStatus(curSpi->SPIx, SPI_I2S_FLAG_RXNE) == RESET) {}

            data = SPI_I2S_ReceiveData(curSpi->SPIx);

            if(recv_ptr != (0))
            {
                *recv_ptr++ = data;
            }
        }
    }
    else if(config->data_width == 16)
    {
        const unsigned short *send_ptr = msg->send_buf;
        unsigned short *recv_ptr = msg->recv_buf;
        unsigned short data = 0xff;

        while(size--)
        {
            if(send_ptr != (0))
            {
                data = *send_ptr++;
            }

            while (SPI_I2S_GetFlagStatus(curSpi->SPIx, SPI_I2S_FLAG_TXE) == RESET) {}

            SPI_I2S_SendData(curSpi->SPIx, data);

            while (SPI_I2S_GetFlagStatus(curSpi->SPIx, SPI_I2S_FLAG_RXNE) == RESET) {}

            data = SPI_I2S_ReceiveData(curSpi->SPIx);

            if(recv_ptr != (0))
            {
                *recv_ptr++ = data;
            }
        }
    }

    // release cs
    if(msg->cs_release && spi_cs)
    {
        GPIO_SetBits(spi_cs->GPIOx,spi_cs->GPIO_Pin);
    }

    return msg->length;
}

int spi_write(struct spi_device *spi_dev,void *data,int len)
{
    struct spi_message message;
    message.send_buf = data;
    message.recv_buf = (0);
    message.length = len;
    message.cs_take = 1;
    message.cs_release = 1;

    return spi_dev->xfer(spi_dev,&message);
}
int spi_read(struct spi_device *spi_dev,void *data,int len)
{
    struct spi_message message;
    message.send_buf = (0);
    message.recv_buf = data;
    message.length = len;
    message.cs_take = 1;
    message.cs_release = 1;

    return spi_dev->xfer(spi_dev,&message);
}

int spi_transfer(struct spi_device *spi_dev,void *send_buf,void *recv_buf,int len)
{
    struct spi_message message;
    message.send_buf = send_buf;
    message.recv_buf = recv_buf;
    message.length = len;
    message.cs_take = 1;
    message.cs_release = 1;

    return spi_dev->xfer(spi_dev,&message);
}

int spi_rdwr_byte(struct spi_device *spi_dev,int data)
{

    int recv_data = 0x00;

#if 1
	    unsigned char no = spi_dev->spi_id % SPI_PORT_MAX;
   	struct SpiPort *curSpi = &systemSpi[no];
	
    while (SPI_I2S_GetFlagStatus(curSpi->SPIx, SPI_I2S_FLAG_TXE) == RESET) {}

    SPI_I2S_SendData(curSpi->SPIx, data);

    while (SPI_I2S_GetFlagStatus(curSpi->SPIx, SPI_I2S_FLAG_RXNE) == RESET) {}

    return SPI_I2S_ReceiveData(curSpi->SPIx);
#else

    spi_dev->transfer(spi_dev,&data,&recv_data,1);
#endif 
    return recv_data;
}

void hardware_spi_init()
{
#ifdef SPI_PORT1_USED
    spi1.init(&spi1);
#endif // SPI_PORT1_USED
}
