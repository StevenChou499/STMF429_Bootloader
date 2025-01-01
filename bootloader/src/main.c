#include "../inc/gpio.h"
#include "../inc/uart.h"
#include "../inc/crc.h"
#include "../inc/bootloader.h"
#include "../inc/myprintf.h"

int main()
{
    bootloader_init();
    
    // read user button to check if directly jump to user application
    if (RESET == User_Btn_Read())
        jump_to_application();

    // if user didn't press the button, wait for bootloader commands
    while (1) {
        myprintf("Waiting for bootloader commands...\r\n");
        parse_bootloader_cmd();
    }
    return 0;
}