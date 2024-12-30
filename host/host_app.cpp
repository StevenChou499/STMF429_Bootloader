#include "host_app.hpp"

#include <string>
#include <cstdint>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <termios.h>
#include <iomanip>

using std::cin, 
      std::cout, 
      std::cerr, 
      std::hex, 
      std::dec, 
      std::setw, 
      std::setfill, 
      std::uppercase, 
      std::endl;

host_app::host_app(string portname)
{
    mcu_fd = open(portname.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    if (mcu_fd < 0) {
        throw std::runtime_error("Error opening corresponding port");
    }
    
    struct termios tty;
    tcgetattr(mcu_fd, &tty);
    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;     // set data bits as 8 bit
    tty.c_cflag &= ~(CSTOPB | PARENB | PARODD | CRTSCTS);
    tty.c_cflag |= (CREAD | CLOCAL);

    tty.c_iflag &= ~(IGNBRK | IXON | IXOFF | IXANY | INLCR | IGNCR | ICRNL);
    tty.c_oflag = 0;
    tty.c_lflag = 0;

    if (tcsetattr(mcu_fd, TCSANOW, &tty) != 0) {
        throw std::runtime_error("Error configuring tty settings");
    }

    tx_cmd_len = rx_cmd_len = mem_read_len = 0U;

    func_table[ 0] = NULL;
    func_table[ 1] = &host_app::write_getver_cmd;
    func_table[ 2] = &host_app::write_gethelp_cmd;
    func_table[ 3] = &host_app::write_getcid_cmd;
    func_table[ 4] = &host_app::write_get_read_prot_cmd;
    func_table[ 5] = &host_app::write_goto_addr_cmd;
    func_table[ 6] = &host_app::write_erase_flash_section_cmd;
    func_table[ 7] = &host_app::write_mem_read_cmd;
    func_table[ 8] = &host_app::write_mem_write_cmd;
    func_table[ 9] = &host_app::write_en_rw_prot_cmd;
    func_table[10] = &host_app::write_dis_rw_prot_cmd;
    func_table[11] = &host_app::write_read_sector_status_cmd;
    func_table[12] = &host_app::write_read_otp_cmd;

    cout << "Welcome for using STM32F4bootloader !" << endl;
}

host_app::~host_app()
{
    if (mcu_fd >= 0)
        close(mcu_fd);
}

void host_app::show_prompt(void)
{
    cout << endl;
    cout << "Bootloader command options: " << endl;
    cout << "Get bootloader version                :  1" << endl;
    cout << "Get bootloader help message           :  2" << endl;
    cout << "Get bootloader chip id                :  3" << endl;
    cout << "Get bootloader read protection status :  4" << endl;
    cout << "Go to specific address                :  5" << endl;
    cout << "Erase flash sections                  :  6" << endl;
    cout << "Memory read                           :  7" << endl;
    cout << "Memory write                          :  8" << endl;
    cout << "Enable read write protection          :  9" << endl;
    cout << "Disable read write protection         : 10" << endl;
    cout << "Read sector status                    : 11" << endl;
    cout << "Read one time programmable memory     : 12" << endl;
    cout << "quit                                  :  q" << endl;
    cout << "Please enter a command: ";
}

bool host_app::get_user_input(void)
{
    string input_value;
    cin >> input_value;

    bool correctness = false;
    try {
        current_cmd = std::stoi(input_value);
        if ((current_cmd >= 1) && (current_cmd <= 12))
            correctness = true;
    } catch (const std::invalid_argument&) {
        if ((input_value.length() == 1) && (input_value[0] == 'q')) {
            current_cmd = 13U;
            correctness = true;
        }
    }
    
    return correctness;
}

bool host_app::parse_command(void)
{
    if (!get_user_input()) {
        cerr << "Error input value, please try again." << endl;
        return true;
    }

    // calls the corresponding function by users input
    if (current_cmd == 13U)
        return false;
    (this->*func_table[current_cmd])();
    return true;
}

void host_app::write_getver_cmd(void)
{
    tx_buffer[0] = 0x1;
    tx_buffer[1] = 0x50;
    tx_cmd_len = 2U;
    append_crc(tx_buffer + 1U, tx_cmd_len - 1U);
    write(mcu_fd, tx_buffer, tx_cmd_len + 4U);
    cout << "Asking for bootloader version..." << endl;
}

void host_app::write_gethelp_cmd(void)
{
    tx_buffer[0] = 0x1;
    tx_buffer[1] = 0x51;
    tx_cmd_len = 2U;
    append_crc(tx_buffer + 1U, tx_cmd_len - 1U);
    write(mcu_fd, tx_buffer, tx_cmd_len + 4U);
    cout << "Asking for help..." << endl;
}

void host_app::write_getcid_cmd(void)
{
    tx_buffer[0] = 0x1;
    tx_buffer[1] = 0x52;
    tx_cmd_len = 2U;
    append_crc(tx_buffer + 1U, tx_cmd_len - 1U);
    write(mcu_fd, tx_buffer, tx_cmd_len + 4U);
    cout << "Asking for chip id..." << endl;
}


/**
 *  1 byte
 * |      |
 * | 0x53 |
 * |      |
 */
void host_app::write_get_read_prot_cmd(void)
{
    tx_buffer[0] = 0x1;
    tx_buffer[1] = 0x53;
    tx_cmd_len = 2U;
    append_crc(tx_buffer + 1U, tx_cmd_len - 1U);
    write(mcu_fd, tx_buffer, tx_cmd_len + 4U);
    cout << "Asking for read protection status..." << endl;
}

/**
 *  1 byte      4 bytes
 * |      |                 |
 * | 0x54 | jumping address |
 * |      |                 |
 */
void host_app::write_goto_addr_cmd(void)
{
    uint32_t jump_addr;
    cout << "What is the address you want to jump to: ";
    cin >> hex >> jump_addr;
    
    tx_buffer[0] = 0x5;
    tx_buffer[1] = 0x54;
    *(uint32_t *)(tx_buffer + 2) = jump_addr;
    tx_cmd_len = 6U;
    append_crc(tx_buffer + 1U, tx_cmd_len - 1U);
    write(mcu_fd, tx_buffer, tx_cmd_len + 4U);
    cout << "Asking for jumping..." << endl;
}

/**
 *  1 byte  1 byte   1 byte
 * |      |        | number |
 * | 0x55 | sector |   of   |
 * |      |   no.  | sector |
 */
void host_app::write_erase_flash_section_cmd(void)
{
    uint32_t sector_number;
    uint32_t number_of_sectors;
    cout << "What is the starting sector you want to erase? : ";
    cin >> sector_number;
    cout << "How many sectors you want to erase? : ";
    cin >> number_of_sectors;

    tx_buffer[0] = 0x3;
    tx_buffer[1] = 0x55;
    tx_buffer[2] = sector_number;
    tx_buffer[3] = number_of_sectors;
    tx_cmd_len = 4U;
    append_crc(tx_buffer + 1U, tx_cmd_len - 1U);
    write(mcu_fd, tx_buffer, tx_cmd_len + 4U);
    cout << "Erasing specific sector..." << endl;
}

/**
 *  1 byte     4 byte     1 byte
 * |      |              |      |
 * | 0x56 | base address | read |
 * |      |              | len  |
 */
void host_app::write_mem_read_cmd(void)
{
    cout << "What is the address you want to read? : ";
    cin >> hex >> mem_read_addr;
    cout << "What is the reading length? (Max 255) : ";
    cin >> dec >> mem_read_len;

    tx_buffer[0] = 0x9;
    tx_buffer[1] = 0x56;
    *(unsigned int *)(tx_buffer + 2) = mem_read_addr;
    *(unsigned int *)(tx_buffer + 6) = mem_read_len;
    tx_cmd_len = 10U;
    append_crc(tx_buffer + 1U, tx_cmd_len - 1U);
    write(mcu_fd, tx_buffer, tx_cmd_len + 4U);
    tcflush(mcu_fd, TCOFLUSH);
    cout << "Reading memory address 0x" << std::setfill('0') 
         << std::hex << mem_read_addr << "..." << endl;
}

void host_app::write_mem_write_cmd(void)
{
    uint32_t base_address;
    std::ifstream binfile;
    cout << "What is the base address you want to program: ";
    cin >> hex >> base_address;
    binfile.open("../User_app_STM32F429xx.bin", std::ios::binary);
    
    while (1) {
        binfile.read((char *) (tx_buffer + 10), 128);
        uint32_t read_len = binfile.gcount();
        if (read_len <= 0)
            break;
        tx_buffer[0] = read_len + 9U;
        tx_buffer[1] = 0x57;
        *(uint32_t *)(tx_buffer + 2) = base_address;
        *(uint32_t *)(tx_buffer + 6) = read_len;
        tx_cmd_len = 10U + read_len;
        append_crc(tx_buffer + 1U, tx_cmd_len - 1U);
        uint32_t written = write(mcu_fd, tx_buffer, tx_cmd_len + 4U);
        usleep(50 * 1000); // wait for 30ms

        r_read(rx_buffer, 1);
        uint8_t ack_or_nack = rx_buffer[0];
        if (ack_or_nack == 0xC) {
            cerr << "CRC Error!" << endl << "Please try again" << endl;
            return;
        }

        r_read(rx_buffer + 1, 1);
        uint8_t program_result;
        program_result = rx_buffer[1];
        if (program_result == 1) {
            cout << "Program memory address 0x" << hex << setw(8) 
                 << setfill('0') << base_address << " success!" << endl;
        } else {
            cout << "Program memory address 0x" << hex << setw(8) 
                 << setfill('0') << base_address << " failed!" << endl;
        }
        
        base_address += read_len; // calculate the next base address
    }

    // Close the Opened binary file
    binfile.close();

    // Program complete
    tx_buffer[0] = 5U;
    tx_buffer[1] = 0x57;
    *(uint32_t *)(tx_buffer + 2) = 0xFFFFFFFFU;
    tx_cmd_len = 6U;
    append_crc(tx_buffer + 1U, tx_cmd_len - 1U);
    write(mcu_fd, tx_buffer, tx_cmd_len + 4U);
    cout << "Program result checking..." << endl;
}

/**
 *  1 byte 1 byte 1 byte
 * |      | prot |      |
 * | 0x58 |  or  | r/w  |
 * |      | not  | prot |
 */
void host_app::write_en_rw_prot_cmd(void)
{
    uint32_t sector_detail = 0U;
    uint8_t protection_mode = 0U;
    uint8_t r_or_w;
    for (uint32_t i = 0U; i < 24U; i++) {
        uint8_t yes_or_no;
        cout << "Do you want to enable protection for sector " 
             << setw(2) << dec << i << "? (Y/N): ";
        cin >> yes_or_no;
        if (yes_or_no == 'N' || yes_or_no == 'n')
            continue;
        sector_detail |= (1U << i);
    }
    cout << "Do you want to enable write or PCROP protection?" << endl;
    cout << "(R: PCROP/ W:Write): ";
    cin >> r_or_w;
    if (r_or_w == 'W' || r_or_w == 'w')
        protection_mode = 1U;
    else
        protection_mode = 2U;
    tx_buffer[0] = 6U;
    tx_buffer[1] = 0x58;
    *(uint32_t *)(tx_buffer + 2) = sector_detail;
    tx_buffer[6] = protection_mode;
    tx_cmd_len = 7U;
    append_crc(tx_buffer + 1U, tx_cmd_len - 1U);
    write(mcu_fd, tx_buffer, tx_cmd_len + 4U);
    cout << "Enabling R/W protection..." << endl;
}

/**
 *  1 byte
 * |      |
 * | 0x59 |
 * |      |
 */
void host_app::write_dis_rw_prot_cmd(void)
{
    tx_buffer[0] = 0x1;
    tx_buffer[1] = 0x59;
    tx_cmd_len = 2U;
    append_crc(tx_buffer + 1U, tx_cmd_len - 1U);
    write(mcu_fd, tx_buffer, tx_cmd_len + 4U);
    cout << "Disabling R/W protection..." << endl;
}

/**
 *  1 byte
 * |      |
 * | 0x5A |
 * |      |
 */
void host_app::write_read_sector_status_cmd(void)
{
    tx_buffer[0] = 0x1;
    tx_buffer[1] = 0x5A;
    tx_cmd_len = 2U;
    append_crc(tx_buffer + 1U, tx_cmd_len - 1U);
    write(mcu_fd, tx_buffer, tx_cmd_len + 4U);
    cout << "Reading sector status..." << endl;
}

/**
 *  1 byte 1 byte
 * |      |       |
 * | 0x5B | block |
 * |      |       |
 */
void host_app::write_read_otp_cmd(void)
{
    uint32_t block_no = 0U;
    cout << "Which block of OTP area do you want to read (0 ~ 15): ";
    cin >> block_no;
    tx_buffer[0] = 0x2;
    tx_buffer[1] = 0x5B;
    tx_buffer[2] = block_no;
    tx_cmd_len = 3U;
    append_crc(tx_buffer + 1U, tx_cmd_len - 1U);
    write(mcu_fd, tx_buffer, tx_cmd_len + 4U);
    cout << "Reading OTP area..." << endl;
}

uint32_t host_app::get_crc(uint8_t *pBuf, uint32_t length)
{
    uint32_t crc_value = 0xFFFFFFFF;
    for (uint32_t i = 0; i < length; i++) {
        crc_value ^= pBuf[i];
        for (uint32_t index = 0; index < 32U; index++) {
            if (crc_value & 0x80000000)
                crc_value = (crc_value << 1) ^ 0x04C11DB7;
            else
                crc_value = (crc_value << 1);
        }
    }
    return crc_value;
}

void host_app::append_crc(uint8_t *pBuf, uint32_t length)
{
    int_or_chars crc_value;
    crc_value.int_value = get_crc(pBuf, length);
    for (uint32_t i = 0U; i < 4U; i++) {
        pBuf[length + i] = crc_value.ch_array[i];
    }
}

void host_app::r_read(uint8_t *pBuf, uint32_t length)
{
    uint32_t total_read_bytes = 0U;
    while (total_read_bytes < length) {
        int bytes_read = read(mcu_fd, 
                              pBuf + total_read_bytes, 
                              length - total_read_bytes);
        total_read_bytes += bytes_read;
    }
}

void host_app::get_bootloader_respond(void)
{
    // read(mcu_fd, rx_buffer, 1);
    r_read(rx_buffer, 1);
    uint8_t ack_or_nack = rx_buffer[0];
    if (ack_or_nack == 0xC) {
        cerr << "CRC Error!" << endl << "Please try again" << endl;
        return;
    }
    switch (current_cmd) {
        case BTLDR_GET_VER:
            uint8_t btldr_version;
            r_read(rx_buffer + 1, 1);
            btldr_version = rx_buffer[1];
            cout << "Bootloader version is ver." << (uint32_t) btldr_version << endl << endl;
            break;
        case BTLDR_GET_HELP:
            uint8_t help_msg[600];
            r_read(help_msg, 572);
            help_msg[572] = '\0';
            cout << endl << "Prompt message: " << endl;
            cout << help_msg << endl;
            break;
        case BTLDR_GET_CID:
            uint16_t revision_id, device_id;
            r_read(rx_buffer + 1, 4);
            revision_id = *(uint16_t *)(rx_buffer + 3);
            device_id = *(uint16_t *)(rx_buffer + 1) & 0xFFF;
            cout << "The chip revision id is " << hex << revision_id
                 << ", and the device id is " << hex << device_id << endl;
            break;
        case BTLDR_GET_RDP:
            uint32_t rdp_status;
            r_read(rx_buffer + 1, 4);
            rdp_status = *(uint32_t *)(rx_buffer + 1);
            cout << "The read protection level is " << rdp_status << endl;
            break;
        case BTLDR_GO_ADDR:
            break;
        case BTLDR_FLASH_ERASE:
            uint8_t erase_result;
            r_read(rx_buffer + 1, 1);
            erase_result = rx_buffer[1];
            if (erase_result == 0x1) {
                cout << "Flash erase successfull!" << endl;
            } else {
                cout << "Flash erase failed!" << endl;
            }
            break;
        case BTLDR_MEM_READ:
            r_read(rx_buffer + 1, mem_read_len);
            for (int i = 0; i < (mem_read_len / 4); i++) {
                cout << setw(8) << setfill('0') << uppercase << hex;
                cout << (mem_read_addr + 4 * i) << ": ";
                cout << setw(8) << setfill('0') << uppercase << hex;
                cout << *((uint32_t *)(rx_buffer + 1) + i) << endl;
            }
            break;
        case BTLDR_MEM_WRITE:
            r_read(rx_buffer + 1, 1);
            uint8_t program_result;
            program_result = rx_buffer[1];
            if (program_result == 1) {
                cout << "Program complete!" << endl;
            } else {
                cout << "Program failed!" << endl;
            }
            break;
        case BTLDR_EN_RW_PROT:
        case BTLDR_DIS_RW_PROT:
            uint8_t config_result;
            r_read(rx_buffer + 1, 1);
            config_result = rx_buffer[1];
            if (config_result == 1) {
                cout << "Config success!" << endl;
            } else {
                cout << "Config failed!" << endl;
            }
            break;
        case BTLDR_READ_SEC_STA:
            uint32_t sector_1, sector_2;
            r_read(rx_buffer + 1, 8);
            sector_1 = *(uint32_t *)(rx_buffer + 1);
            sector_2 = *(uint32_t *)(rx_buffer + 5);
            if (sector_1 & 0x80000000) {
                // PCROP enabled. nWPRi bits are for PCROP protection
                for (int i = 0; i < 12; i++) {
                    cout << setw(2) << dec;
                    cout << "Sector " << i << " PCROP protection: " << 
                    ((sector_1 & (1U << (i + 16))) ? "Enable" : "Disable") 
                    << endl;
                }
                for (int i = 0; i < 12; i++) {
                    cout << setw(2) << dec;
                    cout << "Sector " << i + 12 << " PCROP protection: " << 
                    ((sector_2 & (1U << (i + 16))) ? "Enable" : "Disable") 
                    << endl;
                }
            } else {
                // PCROP disabled. nWPRi bits are for write protection
                for (int i = 0; i < 12; i++) {
                    cout << setw(2) << dec;
                    cout << "Sector " << i << " write protection: " << 
                    ((sector_1 & (1U << (i + 16))) ? "Disable" : "Enable") 
                    << endl;
                }
                for (int i = 0; i < 12; i++) {
                    cout << setw(2) << dec;
                    cout << "Sector " << i + 12 << " write protection: " << 
                    ((sector_2 & (1U << (i + 16))) ? "Disable" : "Enable") 
                    << endl;
                }
            }
            break;
        case BTLDR_READ_OTP:
            uint32_t otp_content[8];
            r_read((uint8_t *)otp_content, 32U);
            for (int i = 0; i < 8; i++) {
                cout << hex << otp_content[i] << endl;
            }
            break;
        default:
            cout << "Unknown command!" << endl;
            break;
    }
}