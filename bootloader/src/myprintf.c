#include "myprintf.h"
#include "uart.h"

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
    unsigned int length = strlen(str);
    UART2_Transmit(str, length);
}