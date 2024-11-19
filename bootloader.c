#include "uart.h"
#include "myprintf.h"
#include "bootloader.h"
#include "crc.h"

btldr_strct_t btldr_strct = {
    .btldr_version = VERSION,
    .btldr_help_msg = 
    "Get bootloader version                :  1\r\n"\
    "Get bootloader help message           :  2\r\n"\
    "Get bootloader chip id                :  3\r\n"\
    "Get bootloader read protection status :  4\r\n"\
    "Go to specific address                :  5\r\n"\
    "Erase flash sections                  :  6\r\n"\
    "Memory read                           :  7\r\n"\
    "Memory write                          :  8\r\n"\
    "Enable read write protection          :  9\r\n"\
    "Disable read write protection         : 10\r\n"\
    "Read sector status                    : 11\r\n"\
    "Read one time programmable memory     : 12\r\n"\
    "quit                                  :  q\r\n",
};

unsigned char recv_buf[128];
unsigned char send_buf[32];

void bootloader_init(void)
{
    
}


// Bootloader command handle function
void parse_bootloader_cmd(void)
{
    // 1. Receive the length of the next command
    unsigned char cmd_len = 0U;
    UART3_Receive(&cmd_len, 1);

    // 2. Receive the actual command code and the CRC
    UART3_Receive(recv_buf, cmd_len + 4);
    unsigned char bl_cmd_code = recv_buf[0];

    // 3. Check the command code
    switch (bl_cmd_code) {
        case BL_GET_VER_CMD:
            myprintf("Received a command ask for version!\r\n");
            bootloader_handle_getver_cmd(recv_buf, cmd_len);
            break;
        case BL_GET_HELP_CMD:
            myprintf("Received a command ask for command options!\r\n");
            bootloader_handle_gethelp_cmd(recv_buf, cmd_len);
            break;
        default:
            myprintf("Unknown command\r\n");
            break;
    }
}

void bootloader_handle_getver_cmd(unsigned char *rx_buffer, unsigned int cmd_len)
{
    unsigned char bl_version = 0U;

    if (CRC_ERROR == CRC32_verify(rx_buffer, cmd_len)) {
        myprintf("Incorrect CRC!\r\n");
        bootloader_send_nack();
    } else {
        myprintf("Correct CRC!\r\n");
        bootloader_send_ack();
        bl_version = get_bootloader_version();
        send_buf[0] = bl_version;
        UART3_Transmit(send_buf, 1U);
    }
}

void bootloader_handle_gethelp_cmd(unsigned char *rx_buffer, unsigned int cmd_len)
{
    if (CRC_ERROR == CRC32_verify(rx_buffer, cmd_len)) {
        myprintf("Incorrect CRC!\r\n");
        bootloader_send_nack();
    } else {
        myprintf("Correct CRC!\r\n");
        bootloader_send_ack();
        unsigned int length = strlen(btldr_strct.btldr_help_msg);
        // const char *str = "Waterburbur\r\n\r\n";
        // unsigned int length = strlen(str);
        UART3_Transmit(btldr_strct.btldr_help_msg, length);
        // UART3_Transmit(str, length);
    }
}

void bootloader_handle_getcid_cmd(unsigned char *rx_buffer, unsigned int cmd_len)
{

}



unsigned char get_bootloader_version(void)
{
    return btldr_strct.btldr_version;
}

unsigned char get_mcu_chip_id(void)
{

}


// Sending ACK and NACK format functions
void bootloader_send_ack(void)
{
    unsigned char ack = ACK;
    UART3_Transmit(&ack, 1U);
}

void bootloader_send_nack(void)
{
    unsigned char nack = NACK;
    UART3_Transmit(&nack, 1U);
}