#include "crc.h"

void CRC32_Init(void)
{
    // Enable the AHB1 clock for CRC unit
    *(volatile unsigned int *) RCC_AHB1ENR |= CRCEN;

    // Reset the CRC calculation unit
    *(volatile unsigned int *) CRC_CR |= CRC_RESET;
    for (int i = 0; i < 10; i++);
    int temp = *(volatile unsigned int *) CRC_DR;
}

unsigned int CRC32_Calculate(unsigned char *pData, unsigned int len)
{
    // Reset the CRC calculation unit
    *(volatile unsigned int *) CRC_CR |= CRC_RESET;

    unsigned int CRC_Value = 0xFF;
    for (int i = 0; i < len; i++) {
        *(volatile unsigned int *) CRC_DR = pData[i];
    }
    return *(volatile unsigned int *) CRC_DR;
    // *(volatile int *) CRC_DR = input;
    // for (int i = 0; i < 10; i++);
    // return *(volatile int *) CRC_DR;
}

unsigned int CRC32_verify(unsigned char *pData, unsigned int len)
{
    unsigned int crc_value = CRC32_Calculate(pData, len);
    unsigned int crc_host = *(unsigned int *) (pData + len);
    if (crc_value == crc_host)
        return CRC_CORRECT;
    else
        return CRC_ERROR;
}