#include "../inc/myprintf.h"
#include "../inc/uart.h"

unsigned int strlen(char *str)
{
    unsigned int length = 0U;
    while (*str++ != '\0') {
        length++;
    }
    return length;
}

int myprintf(const char *str)
{
    unsigned int length = strlen((char *) str);
    UART2_Transmit((unsigned char *) str, length);
    return length;
}