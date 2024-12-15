#include "../inc/flash.h"

unsigned int  flash_unlock(void)
{
    // We need to sequentially write two key values to unlock the flash
    *(volatile unsigned int *)(FLASH_KEYR) = FLASH_UNLOCK_KEY1;
    *(volatile unsigned int *)(FLASH_KEYR) = FLASH_UNLOCK_KEY2;
    if (0x0 == (*(volatile unsigned int *)(FLASH_CR) & (0x1U << 31)))
        return FLASH_SUCCESS;
    else
        return FLASH_FAIL;
}

unsigned int flash_lock(void)
{
    // Writing 1 to the lock bit on flash control register
    *(volatile unsigned int *)(FLASH_CR) |= (0x1U << 31);
    if ((0x1U << 31) == ((*(volatile unsigned int *)(FLASH_CR)) & (0x1U << 31)))
        return FLASH_SUCCESS;
    else
        return FLASH_FAIL;
}

unsigned int flash_sec_erase(unsigned int sec_no)
{
    if (sec_no > (FLASH_SEC_2_START + FLASH_SEC_2_NUM))
        return FLASH_FAIL;

    while (*(volatile unsigned int *)(FLASH_SR) & (0x1U << 16));
    *(volatile unsigned int *)(FLASH_CR) |= (0x1U << 1);
    
    unsigned int snb_bit = 0U;
    if (sec_no < FLASH_SEC_2_START) {
        snb_bit = 0x00 | sec_no;
    } else {
        sec_no -= FLASH_SEC_2_START;
        snb_bit = 0x10 | sec_no;
    }
    *(volatile unsigned int *)(FLASH_CR) |= (snb_bit << 3);
    
    *(volatile unsigned int *)(FLASH_CR) |= (0x1U << 16);
    while (*(volatile unsigned int *)(FLASH_SR) & (0x1U << 16));
    *(volatile unsigned int *)(FLASH_CR) &= ~((0x1F << 3) | (0x1U << 1));
    
    return FLASH_SUCCESS;
}

unsigned int flash_seq_erase(unsigned int start_sec, unsigned int num_sec)
{
    if (FLASH_FAIL == flash_unlock())
        return FLASH_FAIL;

    for (unsigned int index = start_sec; index < start_sec + num_sec; index++) {
        if (index < 12)
            myprintf("Erasing bank 1\r\n");
        else
            myprintf("Erasing bank 2\r\n");
        if (FLASH_FAIL == flash_sec_erase(index))
            return FLASH_FAIL;
    }

    if (FLASH_FAIL == flash_lock())
        return FLASH_FAIL;
    
    return FLASH_SUCCESS;
}