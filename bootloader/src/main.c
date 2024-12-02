#include "../inc/gpio.h"
#include "../inc/uart.h"
#include "../inc/crc.h"
#include "../inc/bootloader.h"
#include "../inc/myprintf.h"

int main()
{
    LD3_Init();
    UART2_Init();
    UART3_Init();
    CRC32_Init();

    for (int i = 0; i < 100; i++);
    // int input = 0x12345678;
    // int output = CRC32_Calculate(&input, 4);
    // unsigned char len = 0U;
    // UART3_Receive(&len, 1);
    // char buffer[128];
    // UART3_Receive(buffer, len);
    // int output = CRC32_Calculate(buffer, len);
    
    // while (1);

    while (1) {
    //     // char receive_buf[20];
    //     // uart3_receive(receive_buf, 1);
    //     // uart3_transmit(receive_buf, 1);
        myprintf("Waiting for bootloader commands...\r\n");
        parse_bootloader_cmd();
    }
    return 0;
}