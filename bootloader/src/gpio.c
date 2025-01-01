#include "../inc/gpio.h"

void LD3_Init(void)
{
    // enable AHB1 bus clock
    *((volatile unsigned int *)(RCC_AHB1ENR)) |= (GPIOBEN);
    // set PB14 to output mode
    *((volatile unsigned int *)(GPIOB)) |= (1 << 28U);
}

void LD3_Toggle(void)
{
    *((volatile unsigned int *)(0x40020414)) ^= (PIN_14);
}

void User_Btn_Init(void)
{
    // enable AHB1 bus clock
    *((volatile unsigned int *)(RCC_AHB1ENR)) |= (GPIOCEN);
    // // set PC13 to input mode
    // *((volatile unsigned int *)(GPIOC)) &= ~(0x3 << 26U);
}

unsigned char User_Btn_Read(void)
{
    unsigned int button_state;
    button_state = *((volatile unsigned int *)(0x40020810U));
    button_state = (button_state & (PIN_13)) >> 13;
    return button_state;
}