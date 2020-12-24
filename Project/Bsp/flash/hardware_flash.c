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

#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ
#define STM32_OTP_BASE  0X1FFF0000 //OTP ��ʼ��ַ
void FlashWrite(unsigned int write_addr,unsigned int *data,unsigned int write_len)
{
    FLASH_Status status = FLASH_COMPLETE;
	unsigned int addrx=0;
	unsigned int endaddr=0;

    if(write_addr < STM32_FLASH_BASE || write_addr % 4)
        return ;	//�Ƿ���ַ

	FLASH_Unlock();									//����
    FLASH_DataCacheCmd(DISABLE);//FLASH�����ڼ�,�����ֹ���ݻ���

	addrx=write_addr;				//д�����ʼ��ַ
	endaddr=write_addr + write_len * 4;	//д��Ľ�����ַ

	if(addrx < STM32_OTP_BASE)			//ֻ�����洢��,����Ҫִ�в�������!!
	{
		while(addrx < endaddr)		//ɨ��һ���ϰ�.(�Է�FFFFFFFF�ĵط�,�Ȳ���)
		{
			if(FlashReadWord(addrx) != 0XFFFFFFFF)//�з�0XFFFFFFFF�ĵط�,Ҫ�����������
			{
				status=FLASH_EraseSector(FlashGetSector(addrx),VoltageRange_3);//VCC=2.7~3.6V֮��!!
				if(status!=FLASH_COMPLETE)break;	//����������
			}
			else
            {
                addrx+=4;
            }
		}
	}
	if(status==FLASH_COMPLETE)
	{
		while(write_addr < endaddr)//д����
		{
			if(FLASH_ProgramWord(write_addr,*data)!=FLASH_COMPLETE)//д������
			{
				break;	//д���쳣
			}
			write_addr+=4;
			data++;
		}
	}

    FLASH_DataCacheCmd(ENABLE);	//FLASH��������,�������ݻ���
	FLASH_Lock();//����
}

void FlashRead(unsigned int read_addr,unsigned int *data,unsigned int read_len)
{
    unsigned int i;
    for(i = 0; i < read_len;i++)
    {
        data[i] = FlashReadWord(read_addr);
        read_addr += 4;//ƫ��4�ֽ�
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

