#include "../inc/uart.h"

void UART2_Init(void)
{
    // Inorder to initialize USART2, we need to enable GPIO and USART2 clock
    *((volatile int *) RCC_AHB1ENR) |= GPIOAEN;  // enable AHB1 for GPIOA
    *((volatile int *) RCC_APB1ENR) |= USART2EN; // enable APB1 for USART2

    *((volatile int *) GPIOA_MODER) |= (ALT_FUNC_MODE << (2 * 2)); // set PA2 to alt. mode
    *((volatile int *) GPIOA_MODER) |= (ALT_FUNC_MODE << (2 * 3)); // set PA3 to alt. mode

    *((volatile int *) GPIOA_AFRL) |= (ALT_FUNC_7 << (2 * 4)); // set PA2 to AF7
    *((volatile int *) GPIOA_AFRL) |= (ALT_FUNC_7 << (3 * 4)); // set PA3 to AF7

    *((volatile int *) USART2_BRR) = 0x8B; // configure baud rate to 115200
    *((volatile int *) USART2_CR1) |= USART_TE; // enable Transmitter
    *((volatile int *) USART2_CR1) |= USART_RE; // enable Receiver
    *((volatile int *) USART2_CR1) |= USART_EN; // enable USART2
}

void UART2_Transmit(const unsigned char *buf, int len)
{
    for (int i = 0; i < len; i++) {
        while (!((*(volatile int *) USART2_SR) & USART_TXE));
        *((volatile int *) USART2_DR) = buf[i];
        while (!(*((volatile int *) USART2_SR) & USART_TC));
    }
}

void UART2_Receive(unsigned char *buf, int len)
{
    for (int i = 0; i < len; i++) {
        while (!(*((volatile int *) USART2_SR) & USART_RXNE));
        buf[i] = *((volatile int *) USART2_DR);
    }
}

void UART3_Init(void)
{
    // Inorder to initialize USART3, we need to enable GPIO and USART3 clock
    *((volatile int *) RCC_AHB1ENR) |= GPIODEN;  // enable AHB1 for GPIOD
    *((volatile int *) RCC_APB1ENR) |= USART3EN; // enable APB1 for USART3

    *((volatile int *) GPIOD_MODER) |= (ALT_FUNC_MODE << (2 * 8)); // set PD8 to alt. mode
    *((volatile int *) GPIOD_MODER) |= (ALT_FUNC_MODE << (2 * 9)); // set PD9 to alt. mode

    *((volatile int *) GPIOD_AFRH) |= (ALT_FUNC_7 << (0 * 4)); // set PD8 to AF7
    *((volatile int *) GPIOD_AFRH) |= (ALT_FUNC_7 << (1 * 4)); // set PD9 to AF7

    *((volatile int *) USART3_BRR) = 0x8B; // configure baud rate to 115200
    *((volatile int *) USART3_CR1) |= USART_TE; // enable Transmitter
    *((volatile int *) USART3_CR1) |= USART_RE; // enable Receiver
    *((volatile int *) USART3_CR1) |= USART_EN; // enable USART3
}

void UART3_Transmit(const unsigned char *buf, int len)
{
    for (int i = 0; i < len; i++) {
        while (!((*(volatile int *) USART3_SR) & USART_TXE));
        *((volatile int *) USART3_DR) = buf[i];
        while (!(*((volatile int *) USART3_SR) & USART_TC));
    }
}

void UART3_Receive(unsigned char *buf, int len)
{
    for (int i = 0; i < len; i++) {
        while (!(*((volatile int *) USART3_SR) & USART_RXNE));
        buf[i] = *((volatile int *) USART3_DR);
    }
}