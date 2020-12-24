#ifndef HARDWARE_FLASH_H
#define HARDWARE_FLASH_H

typedef enum
{
    FLASH_SECTOR_0,
    FLASH_SECTOR_1,
    FLASH_SECTOR_2,
    FLASH_SECTOR_3,
    FLASH_SECTOR_4,
    FLASH_SECTOR_5,
    FLASH_SECTOR_6,
    FLASH_SECTOR_7,
    FLASH_SECTOR_8,
    FLASH_SECTOR_9,
    FLASH_SECTOR_10,
    FLASH_SECTOR_11,
    FLASH_SECTOR_MAX
}flash_sector_e;

struct flash_blk
{
    flash_sector_e sector_id;
    unsigned int start_addr;
    unsigned int end_addr;
    unsigned int sector_size;
};

typedef struct flash_device
{
    struct flash_blk blocks[FLASH_SECTOR_MAX];

    void (*Write)(unsigned int write_addr,unsigned int *data,unsigned int write_len);
    void (*Read)(unsigned int read_addr,unsigned int *data,unsigned int read_len);
    unsigned int (*ReadWord)(unsigned int addr);
    unsigned short (*EraseAll)(void);
    unsigned short (*EraseSector)(unsigned char sector);
    unsigned short (*GetSector)(unsigned int addr);
}flash_dev_t;

extern flash_dev_t on_chip_flash;

void hardware_flash_init(void);

#endif // HARDWARE_FLASH_H
