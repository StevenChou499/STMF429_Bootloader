#include "../inc/gpio.h"

void LD3_Init(void)
{
    // 1. enable AHB1 bus clock
    *((volatile int *)(0x40023830)) |= (1 << 1U);
    *((volatile int *)(0x40020400)) |= (1 << 28U);
}

void LD3_Toggle(void)
{
    *((volatile int *)(0x40020414)) ^= (1 << 14U);
}