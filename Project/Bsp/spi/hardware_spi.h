#ifndef HARDWARE_SPI_H
#define HARDWARE_SPI_H


#define SPI_SPI_CPHA     (1<<0)                             /* bit[0]:CPHA, clock phase 相位*/
#define SPI_SPI_CPOL     (1<<1)                             /* bit[1]:CPOL, clock polarity 极性*/

#define SPI_SPI_LSB      (0<<2)                             /* bit[2]: 0-LSB 低位有效*/
#define SPI_SPI_MSB      (1<<2)                             /* bit[2]: 1-MSB */

#define SPI_SPI_MASTER   (0<<3)                             /* SPI master device */
#define SPI_SPI_SLAVE    (1<<3)                             /* SPI slave device */

#define SPI_SPI_MODE_0       (0 | 0)                        /* CPOL = 0, CPHA = 0 */
#define SPI_SPI_MODE_1       (0 | SPI_SPI_CPHA)                 /* CPOL = 0, CPHA = 1 */
#define SPI_SPI_MODE_2       (SPI_SPI_CPOL | 0)                 /* CPOL = 1, CPHA = 0 */
#define SPI_SPI_MODE_3       (SPI_SPI_CPOL | SPI_SPI_CPHA)          /* CPOL = 1, CPHA = 1 */

#define SPI_MODE_MASK    (SPI_SPI_CPHA | SPI_SPI_CPOL | SPI_SPI_MSB)

#define SPI_SPI_CS_HIGH  (1<<4)                             /* Chipselect active high */
#define SPI_SPI_NO_CS    (1<<5)                             /* No chipselect */
#define SPI_SPI_3WIRE    (1<<6)                             /* SI/SO pin shared */
#define SPI_SPI_READY    (1<<7)                             /* Slave pulls low to pause */

/**
* spi message struct
*/
struct spi_message
{
    const void *send_buf;
    void *recv_buf;
    unsigned long length;

    unsigned char cs_take:1; //已经被片选
    unsigned char cs_release:1;//片选释放
};

/**
* spi config struct
*/
struct spi_config
{
    unsigned char mode;
    unsigned char data_width;
    unsigned int max_hz;
};

typedef enum
{
    SPI_PORT_1,
    SPI_PORT_2,
    SPI_PORT_MAX
}spi_port_e;

struct spi_device
{
    spi_port_e spi_id;
    struct spi_config config;
    void *user_data;//FOR CS PIN
    void (*init)(struct spi_device *spi_dev);
    void (*configure)(struct spi_device *spi_dev,struct spi_config *config);
    unsigned long (*xfer)(struct spi_device *spi_dev,struct spi_message *msg);
    //void (*cs_select)(struct spi_device *spi_dev);
   // void (*cs_unselect)(struct spi_device *spi_dev);
		
	  //only write not read  Action!
    int (*write)(struct spi_device *spi_dev,void *data,int len);
	  //only read not write
    int (*read)(struct spi_device *spi_dev,void *data,int len);
    int (*transfer)(struct spi_device *spi_dev,void *send_buf,void *recv_buf,int len);
    int (*rdwr_byte)(struct spi_device *spi_dev,int data);
};

#define SPI_PORT1_USED	1
#ifdef SPI_PORT1_USED
    extern struct spi_device spi1;
#endif

void hardware_spi_init(void);

#endif // HARDWARE_SPI_H
