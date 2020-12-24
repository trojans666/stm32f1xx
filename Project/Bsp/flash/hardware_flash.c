#include "hardware_flash.h"
#include "stm32f4xx_flash.h"

#define M8(adr)  (*((volatile unsigned char  *) (adr)))
#define M16(adr) (*((volatile unsigned short *) (adr)))
#define M32(adr) (*((volatile unsigned long  *) (adr)))


void FlashWrite(unsigned int write_addr,unsigned int *data,unsigned int write_len);
void FlashRead(unsigned int read_addr,unsigned int *data,unsigned int read_len);
unsigned int FlashReadWord(unsigned int addr);
unsigned short FlashEraseAll(void);
unsigned short FlashEraseSector(unsigned char sector);
unsigned short FlashGetSector(unsigned int addr);


flash_dev_t on_chip_flash =
{
		{
			{
        FLASH_SECTOR_0,
        0x08000000,
        0x08003FFF,
        16 * 1024
			},
			{
        FLASH_SECTOR_1,
        0x08004000,
        0x08007FFF,
        16 * 1024
			},
			{
        FLASH_SECTOR_2,
        0x08008000,
        0x0800BFFF,
        16 * 1024
			},
			{
        FLASH_SECTOR_3,
        0x0800C000,
        0x0800FFFF,
        16 * 1024
			},
			{
        FLASH_SECTOR_4,
        0x08010000,
        0x0801FFFF,
        64 * 1024
			},
			{
        FLASH_SECTOR_5,
        0x08020000,
        0x0803FFFF,
        128 * 1024
			},
			{
        FLASH_SECTOR_6,
        0x08040000,
        0x0805FFFF,
        128 * 1024
			},
			{
        FLASH_SECTOR_7,
        0x08060000,
        0x0807FFFF,
        128 * 1024
			},
			{
        FLASH_SECTOR_8,
        0x08080000,
        0x0809FFFF,
        128 * 1024
			},
			{
        FLASH_SECTOR_9,
        0x080A0000,
        0x080BFFFF,
        128 * 1024
			},
			{
        FLASH_SECTOR_10,
        0x080C0000,
        0x080DFFFF,
        128 * 1024
			},
			{
        FLASH_SECTOR_11,
        0x080E0000,
        0x080FFFFF,
        128 * 1024
			}
		},
    FlashWrite,
    FlashRead,
    FlashReadWord,
    FlashEraseAll,
    FlashEraseSector,
    FlashGetSector
};

#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH的起始地址
#define STM32_OTP_BASE  0X1FFF0000 //OTP 起始地址
void FlashWrite(unsigned int write_addr,unsigned int *data,unsigned int write_len)
{
    FLASH_Status status = FLASH_COMPLETE;
	unsigned int addrx=0;
	unsigned int endaddr=0;

    if(write_addr < STM32_FLASH_BASE || write_addr % 4)
        return ;	//非法地址

	FLASH_Unlock();									//解锁
    FLASH_DataCacheCmd(DISABLE);//FLASH擦除期间,必须禁止数据缓存

	addrx=write_addr;				//写入的起始地址
	endaddr=write_addr + write_len * 4;	//写入的结束地址

	if(addrx < STM32_OTP_BASE)			//只有主存储区,才需要执行擦除操作!!
	{
		while(addrx < endaddr)		//扫清一切障碍.(对非FFFFFFFF的地方,先擦除)
		{
			if(FlashReadWord(addrx) != 0XFFFFFFFF)//有非0XFFFFFFFF的地方,要擦除这个扇区
			{
				status=FLASH_EraseSector(FlashGetSector(addrx),VoltageRange_3);//VCC=2.7~3.6V之间!!
				if(status!=FLASH_COMPLETE)break;	//发生错误了
			}
			else
            {
                addrx+=4;
            }
		}
	}
	if(status==FLASH_COMPLETE)
	{
		while(write_addr < endaddr)//写数据
		{
			if(FLASH_ProgramWord(write_addr,*data)!=FLASH_COMPLETE)//写入数据
			{
				break;	//写入异常
			}
			write_addr+=4;
			data++;
		}
	}

    FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
	FLASH_Lock();//上锁
}

void FlashRead(unsigned int read_addr,unsigned int *data,unsigned int read_len)
{
    unsigned int i;
    for(i = 0; i < read_len;i++)
    {
        data[i] = FlashReadWord(read_addr);
        read_addr += 4;//偏移4字节
    }
}

unsigned int FlashReadWord(unsigned int addr)
{
    return *(unsigned int *)addr;
}

unsigned short FlashEraseAll(void)
{
	return 0;
}

unsigned short FlashEraseSector(unsigned char sector)
{
		return 0;
}

unsigned short FlashGetSector(unsigned int addr)
{
    int pos = 0;
    if(addr < on_chip_flash.blocks[pos++].start_addr)
        return FLASH_SECTOR_0;
    else if(addr < on_chip_flash.blocks[pos++].start_addr)
        return FLASH_SECTOR_1;
    else if(addr < on_chip_flash.blocks[pos++].start_addr)
        return FLASH_SECTOR_2;
    else if(addr < on_chip_flash.blocks[pos++].start_addr)
        return FLASH_SECTOR_3;
    else if(addr < on_chip_flash.blocks[pos++].start_addr)
        return FLASH_SECTOR_4;
    else if(addr < on_chip_flash.blocks[pos++].start_addr)
        return FLASH_SECTOR_5;
    else if(addr < on_chip_flash.blocks[pos++].start_addr)
        return FLASH_SECTOR_6;
    else if(addr < on_chip_flash.blocks[pos++].start_addr)
        return FLASH_SECTOR_7;
    else if(addr < on_chip_flash.blocks[pos++].start_addr)
        return FLASH_SECTOR_8;
    else if(addr < on_chip_flash.blocks[pos++].start_addr)
        return FLASH_SECTOR_9;
    else if(addr < on_chip_flash.blocks[pos++].start_addr)
        return FLASH_SECTOR_10;

    return FLASH_SECTOR_11;
}


void hardware_flash_init()
{

}

