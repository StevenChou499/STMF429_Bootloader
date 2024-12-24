#include "../inc/uart.h"
#include "../inc/myprintf.h"
#include "../inc/bootloader.h"
#include "../inc/crc.h"
#include "../inc/flash.h"

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
        case BL_GET_CID_CMD:
            myprintf("Received a command ask for chip id!\r\n");
            bootloader_handle_getcid_cmd(recv_buf, cmd_len);
            break;
        case BL_GET_RDP_STATUS_CMD:
            myprintf("Received a command ask for read protection status!\r\n");
            bootloader_handle_getrdp_status_cmd(recv_buf, cmd_len);
            break;
        case BL_GET_GO_TO_ADDR_CMD:
            myprintf("Received a command jumping to specific address!\r\n");
            bootloader_handle_goaddr_cmd(recv_buf, cmd_len);
            break;
        case BL_FLASH_ERASE_CMD:
            myprintf("Received a command ask to erase flash!\r\n");
            bootloader_handle_flash_erase_cmd(recv_buf, cmd_len);
            break;
        case BL_MEM_READ_CMD:
            myprintf("Received a command ask to read memory!\r\n");
            break;
        case BL_MEM_WRITE_CMD:
            myprintf("Received a command ask to write memory!\r\n");
            break;
        case BL_EN_RW_PROTECTION_CMD:
            myprintf("Received a command ask to enable r/w protection!\r\n");
            bootloader_handle_en_rw_prot_cmd(recv_buf, cmd_len);
            break;
        case BL_DIS_RW_PROTECTION_CMD:
            myprintf("Received a command ask to disable r/w protection!\r\n");
            bootloader_handle_dis_rw_prot_cmd(recv_buf, cmd_len);
            break;
        case BL_READ_SECTOR_STATUS_CMD:
            myprintf("Received a command ask to read sector status!\r\n");
            bootloader_handle_read_sector_status_cmd(recv_buf, cmd_len);
            break;
        case BL_OTP_READ_CMD:
            myprintf("Received a command to read one time programmable memory!\r\n");
            bootloader_handle_read_otp_cmd(recv_buf, cmd_len);
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
        unsigned int length = strlen((char *) btldr_strct.btldr_help_msg);
        UART3_Transmit((unsigned char *) btldr_strct.btldr_help_msg, length);
    }
}

void bootloader_handle_getcid_cmd(unsigned char *rx_buffer, unsigned int cmd_len)
{
    if (CRC_ERROR == CRC32_verify(rx_buffer, cmd_len)) {
        myprintf("Incorrect CRC!\r\n");
        bootloader_send_nack();
    } else {
        myprintf("Correct CRC!\r\n");
        bootloader_send_ack();
        unsigned int id_code = get_mcu_chip_id();
        UART3_Transmit((unsigned char *) &id_code, 4U);
    }
}

void bootloader_handle_getrdp_status_cmd(unsigned char *rx_buffer, unsigned int cmd_len)
{
    if (CRC_ERROR == CRC32_verify(rx_buffer, cmd_len)) {
        myprintf("Incorrect CRC!\r\n");
        bootloader_send_nack();
    } else {
        myprintf("Correct CRC!\r\n");
        bootloader_send_ack();
        unsigned int rdp_status = get_rdp_status();
        UART3_Transmit((unsigned char *) &rdp_status, 4U);
    }
}

void bootloader_handle_goaddr_cmd(unsigned char *rx_buffer, unsigned int cmd_len)
{
    return;
}

void bootloader_handle_flash_erase_cmd(unsigned char *rx_buffer, unsigned int cmd_len)
{
    if (CRC_ERROR == CRC32_verify(rx_buffer, cmd_len)) {
        myprintf("Incorrect CRC!\r\n");
        bootloader_send_nack();
    } else {
        myprintf("Correct CRC!\r\n");
        bootloader_send_ack();
        unsigned char starting_sector = rx_buffer[1];
        unsigned char num_of_sectors = rx_buffer[2];
        unsigned char erase_status = flash_seq_erase(starting_sector, num_of_sectors);
        UART3_Transmit((unsigned char *) &erase_status, 1U);
    }
}

void bootloader_handle_en_rw_prot_cmd(unsigned char *rx_buffer, unsigned int cmd_len)
{
    if (CRC_ERROR == CRC32_verify(rx_buffer, cmd_len)) {
        myprintf("Incorrect CRC!\r\n");
        bootloader_send_nack();
    } else {
        myprintf("Correct CRC!\r\n");
        bootloader_send_ack();
        unsigned int sector_detail = *(unsigned int *) (rx_buffer + 1);
        unsigned int protection_mode = rx_buffer[5];
        unsigned char result = configure_flash_sector_rw_prot(sector_detail, protection_mode);
        UART3_Transmit((unsigned char *) &result, 1U);
    }
}

void bootloader_handle_dis_rw_prot_cmd(unsigned char *rx_buffer, unsigned int cmd_len)
{
    if (CRC_ERROR == CRC32_verify(rx_buffer, cmd_len)) {
        myprintf("Incorrect CRC!\r\n");
        bootloader_send_nack();
    } else {
        myprintf("Correct CRC!\r\n");
        bootloader_send_ack();
        unsigned char result = configure_flash_sector_rw_prot(0xFFFFFF, 0U);
        UART3_Transmit((unsigned char *) &result, 1U);
    }
}

void bootloader_handle_read_sector_status_cmd(unsigned char *rx_buffer, unsigned int cmd_len)
{
    if (CRC_ERROR == CRC32_verify(rx_buffer, cmd_len)) {
        myprintf("Incorrect CRC!\r\n");
        bootloader_send_nack();
    } else {
        myprintf("Correct CRC!\r\n");
        bootloader_send_ack();
        unsigned int sector_status[2];
        get_sector_status(sector_status, sector_status + 1);
        UART3_Transmit((unsigned char *) sector_status, 8U);
    }
}

void bootloader_handle_read_otp_cmd(unsigned char *rx_buffer, unsigned int cmd_len)
{
    if (CRC_ERROR == CRC32_verify(rx_buffer, cmd_len)) {
        myprintf("Incorrect CRC!\r\n");
        bootloader_send_nack();
    } else {
        myprintf("Correct CRC!\r\n");
        bootloader_send_ack();
        unsigned int reading_block = rx_buffer[1];
        unsigned char otp_content[32];
        read_otp_sector(reading_block, otp_content);
        UART3_Transmit(otp_content, 32U);
    }
}

unsigned char get_bootloader_version(void)
{
    return btldr_strct.btldr_version;
}

unsigned int get_mcu_chip_id(void)
{
    unsigned int id_code = *(volatile unsigned int *)(DBGMCU_IDCODE_ADDR);
    return id_code;
}

unsigned int get_rdp_status(void)
{
    unsigned int rdp_option_bytes = *(volatile unsigned int *)(0x1FFFC000);
    rdp_option_bytes = (rdp_option_bytes & 0x0000FF00) >> 8;
    if (rdp_option_bytes == 0xAA) {
        return 0U;
    } else if (rdp_option_bytes == 0xCC) {
        return 2U;
    } else {
        return 1U;
    }
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