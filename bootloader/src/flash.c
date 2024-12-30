#include "../inc/flash.h"
#include "../inc/myprintf.h"

unsigned int flash_unlock(void)
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

unsigned int option_byte_unlock(void)
{
    // We need to sequentially write two key values to unlock option bytes
    *(volatile unsigned int *)(FLASH_OPTKEYR) = OB_UNLOCK_KEY1;
    *(volatile unsigned int *)(FLASH_OPTKEYR) = OB_UNLOCK_KEY2;
    if (0x0 == (*(volatile unsigned int *)(FLASH_OPTCR) & (0x1U << 0)))
        return FLASH_SUCCESS;
    else
        return FLASH_FAIL;
}

unsigned int option_byte_lock(void)
{
    *(volatile unsigned int *)(FLASH_OPTCR) |= (0x1U << 0);
    if ((0x1U << 0) == (*(volatile unsigned int *)(FLASH_OPTCR) & (0x1U << 0)))
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
    *(volatile unsigned int *)(FLASH_CR) &= ~(0x1F << 3);
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

unsigned int flash_program(unsigned int start_addr, unsigned char *pBuf, unsigned int len)
{
    
    // flash_sec_erase(7U); // erasing sector 7, which is address 0x08060000 ~ 0x0807FFFF
    // check BSY bit in FLASH_SR register
    if (FLASH_FAIL == flash_unlock())
        return FLASH_FAIL;
    while (*(volatile unsigned int *)(FLASH_SR) & (0x1U << 16));
    
    // set PG bit in FLASH_CR register
    *(unsigned int *)(FLASH_CR) |= (1U << 0);
    // perform data write operation to desired memory address
    for (int i = 0; i < len; i++) {
        *(unsigned char *)(start_addr + i) = pBuf[i];
    }
    // wait for BSY bit to be cleared
    while (*(volatile unsigned int *)(FLASH_SR) & (0x1U << 16));

    if (FLASH_FAIL == flash_lock())
        return FLASH_FAIL;
    
    return FLASH_SUCCESS;
}

unsigned int configure_flash_sector_rw_prot(unsigned int sector_details, unsigned char prot_mode)
{
    if (FLASH_FAIL == option_byte_unlock())
        return FLASH_FAIL;
    while (*(volatile unsigned int *)(FLASH_SR) & (0x1U << 16));

    unsigned int sector_1_detail = sector_details & 0xFFF;
    unsigned int sector_2_detail = (sector_details & 0xFFF000) >> 12;

    if (prot_mode == 0U) { //disable
        // reset SPRMOD bit to default state
        *(unsigned int *)(FLASH_OPTCR)  &= ~(1U << 31);
        // reset nWRP bit to default state
        *(unsigned int *)(FLASH_OPTCR1) |= (0xFFF << 16);
        *(unsigned int *)(FLASH_OPTCR)  |= (0xFFF << 16);
        // set OPTSTRT bit for operation
        *(unsigned int *)(FLASH_OPTCR)  |= (1U << 1);
    } else if (prot_mode == 1U) { // write protection
        // set SPRMOD bit to 0 for write protection
        *(unsigned int *)(FLASH_OPTCR)  &= ~(1U << 31);
        // configure nWRP bit for specific bits
        *(unsigned int *)(FLASH_OPTCR1) |= (0xFFF << 16);
        *(unsigned int *)(FLASH_OPTCR1) &= ~(sector_2_detail << 16);
        *(unsigned int *)(FLASH_OPTCR)  |= (0xFFF << 16);
        *(unsigned int *)(FLASH_OPTCR)  &= ~(sector_1_detail << 16);
        // set OPTSTRT bit for operation
        *(unsigned int *)(FLASH_OPTCR)  |= (1U << 1);
    } else { // PCROP protection
        // set SPRMOD bit to 1 for PCROP protection
        *(unsigned int *)(FLASH_OPTCR)  |= (1U << 31);
        // configure nWRP bit for specific bits
        *(unsigned int *)(FLASH_OPTCR1) &= ~(0xFFF << 16);
        *(unsigned int *)(FLASH_OPTCR1) |= (sector_2_detail << 16);
        *(unsigned int *)(FLASH_OPTCR)  &= ~(0xFFF << 16);
        *(unsigned int *)(FLASH_OPTCR)  |= (sector_1_detail << 16);
        //  set OPTSTRT bit for operation
        *(unsigned int *)(FLASH_OPTCR)  |= (1U << 1);
    }

    while (*(volatile unsigned int *)(FLASH_SR) & (0x1U << 16));
    if (FLASH_FAIL == option_byte_lock())
        return FLASH_FAIL;
    return FLASH_SUCCESS;
}

unsigned int get_sector_status(unsigned int *sector_1, unsigned int *sector_2)
{
    if (FLASH_FAIL == option_byte_unlock())
        return FLASH_FAIL;
    while (*(volatile unsigned int *)(FLASH_SR) & (0x1U << 16));

    // return the flash bank 1 & 2 nWRP bits and SPRMOD bits
    *sector_1 = *(volatile unsigned int *)(FLASH_OPTCR)  & (0x8FFF0000);
    *sector_2 = *(volatile unsigned int *)(FLASH_OPTCR1) & (0x0FFF0000);

    while (*(volatile unsigned int *)(FLASH_SR) & (0x1U << 16));
    if (FLASH_FAIL == option_byte_lock())
        return FLASH_FAIL;
    return FLASH_SUCCESS;
}

void read_otp_sector(unsigned int block_no, unsigned char *storing_buf)
{
    if (block_no > 15)
        return;
    unsigned char *pOTP = ((unsigned char *)(FLASH_OTP_BASE) + 32U * block_no);
    for (int i = 0; i < 32; i++) {
        storing_buf[i] = pOTP[i];
    }
}