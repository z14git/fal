/*
 * File      : fal_flash_stm32f2_port.c
 * COPYRIGHT (C) 2012-2018, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-01-26     armink       the first version
 */

#include <fal.h>

#include <board.h>

/* base address of the flash sectors */

/**
 * Get the sector of a given address
 *
 * @param address flash address
 *
 * @return The sector of a given address
 */
static uint32_t stm32_get_sector(uint32_t address)
{
    return 0;
}

/**
 * Get the sector size
 *
 * @param sector sector
 *
 * @return sector size
 */
static uint32_t stm32_get_sector_size(uint32_t sector)
{
    return 1024;
}

static int init(void)
{
    /* do nothing now */
    return 0;
}

static int read(long offset, uint8_t *buf, size_t size)
{
    uint32_t addr = stm32f2_onchip_flash.addr + offset;
    for (; size > 0; size -= 1, addr += 1, buf++)
    {
        *buf = *(uint8_t *)addr;
    }

    return size;
}

static int write(long offset, const uint8_t *buf, size_t size)
{
    size_t            i;
    uint32_t          read_data;
    uint32_t          addr = stm32f2_onchip_flash.addr + offset;
    HAL_StatusTypeDef flashStatus;

    HAL_FLASH_Unlock();
    for (i = 0; i < size; i += 2, buf += 2, addr += 2)
    {
        /* write data */
        // FLASH_ProgramByte(addr, *buf);
        flashStatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr,
                                        *(uint16_t *)buf);
        read_data   = *(uint16_t *)addr;
        /* check data */
        if (flashStatus != HAL_OK)
        {
            return -1;
        }
        if (read_data != *(uint16_t *)buf)
        {
            return -1;
        }
    }
    HAL_FLASH_Lock();

    return size;
}

static int erase(long offset, size_t size)
{
    HAL_StatusTypeDef flash_status;
    size_t            erased_size = 0;
    // uint32_t               cur_erase_sector;
    uint32_t               addr       = stm32f2_onchip_flash.addr + offset;
    uint32_t               page_error = 0;
    FLASH_EraseInitTypeDef s_eraseinit;
    s_eraseinit.TypeErase   = FLASH_TYPEERASE_PAGES;
    s_eraseinit.PageAddress = addr;
    s_eraseinit.NbPages     = 1;

    /* start erase */
    HAL_FLASH_Unlock();
    /* it will stop when erased size is greater than setting size */
    while (erased_size < size)
    {
        // cur_erase_sector = stm32_get_sector(addr + erased_size);
        flash_status = HAL_FLASHEx_Erase(&s_eraseinit, &page_error);
        if (flash_status != HAL_OK)
        {
            return -1;
        }
        // erased_size += stm32_get_sector_size(cur_erase_sector);
        erased_size += stm32_get_sector_size(0);
        s_eraseinit.PageAddress += stm32_get_sector_size(0);
    }
    HAL_FLASH_Lock();

    return size;
}

const struct fal_flash_dev stm32f2_onchip_flash = {"stm32_onchip",
                                                   0x08000000,
                                                   64 * 1024,
                                                   1 * 1024,
                                                   {init, read, write, erase}};
