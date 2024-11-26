#include <string>
#include <cstdint>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <termios.h>

#define TX_BUFFER_LEN (1024U)
#define RX_BUFFER_LEN (128U)

using std::string, 
      std::cin, 
      std::cout, 
      std::cerr, 
      std::hex, 
      std::endl;
    
typedef union {
    uint32_t int_value;
    uint8_t  ch_array[4];
} int_or_chars;

class host_app {
public:
    typedef void (host_app::*functionpointer)();
    int32_t mcu_fd;
    uint32_t users_choice;
    uint8_t tx_buffer[TX_BUFFER_LEN];
    uint8_t rx_buffer[RX_BUFFER_LEN];
    uint32_t tx_cmd_len;
    uint32_t rx_cmd_len;
    functionpointer func_table[13];

    void write_getver_cmd(void);
    void write_gethelp_cmd(void);
    void write_getcid_cmd(void);
    void write_get_read_prot_cmd(void);
    void write_goto_addr_cmd(void);
    void write_erase_flash_section_cmd(void);
    void write_mem_read_cmd(void);
    void write_mem_write_cmd(void);
    void write_en_rw_prot_cmd(void);
    void write_dis_rw_prot_cmd(void);
    void write_read_sector_status_cmd(void);
    void write_read_otp_cmd(void);

    uint32_t get_crc(uint8_t *pBuf, uint32_t length);
    void append_crc(uint8_t *pBuf, uint32_t length);
        
        
public:
    host_app(string portname);
    ~host_app();
    void show_prompt(void);
    bool get_user_input(void);
    void parse_command(void);
};

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

    func_table[0]  = NULL;
    func_table[1]  = &host_app::write_getver_cmd;
    func_table[2]  = &host_app::write_gethelp_cmd;
    func_table[3]  = &host_app::write_getcid_cmd;
    func_table[4]  = &host_app::write_get_read_prot_cmd;
    func_table[5]  = &host_app::write_goto_addr_cmd;
    func_table[6]  = &host_app::write_erase_flash_section_cmd;
    func_table[7]  = &host_app::write_mem_read_cmd;
    func_table[8]  = &host_app::write_mem_write_cmd;
    func_table[9]  = &host_app::write_en_rw_prot_cmd;
    func_table[10] = &host_app::write_dis_rw_prot_cmd;
    func_table[11] = &host_app::write_read_sector_status_cmd;
    func_table[12] = &host_app::write_read_otp_cmd;

    cout << "Welcome for using STM32F4bootloader !" << endl;
    cout << endl;
}

host_app::~host_app()
{
    if (mcu_fd >= 0)
        close(mcu_fd);
}

void host_app::show_prompt(void)
{
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
}

bool host_app::get_user_input(void)
{
    string input_value;
    cin >> input_value;

    bool correctness = false;
    try {
        users_choice = std::stoi(input_value);
        if ((users_choice >= 1) && (users_choice <= 12))
            correctness = true;
    } catch (const std::invalid_argument&) {
        if ((input_value.length() == 1) && (input_value[0] == 'q')) {
            users_choice = 13U;
            correctness = true;
        }
    }
    
    return correctness;
}

void host_app::parse_command(void)
{
    if (!get_user_input()) {
        cerr << "Error input value, please try again." << endl;
        return;
    }

    // calls the corresponding function by users input
    (this->*func_table[users_choice])();
    append_crc(tx_buffer, tx_cmd_len);
}

void host_app::write_getver_cmd(void)
{
    tx_buffer[0] = 0x50;
    tx_cmd_len = 1U;
}

void host_app::write_gethelp_cmd(void)
{
    tx_buffer[0] = 0x51;
    tx_cmd_len = 1U;
}

void host_app::write_getcid_cmd(void)
{
    tx_buffer[0] = 0x52;
    tx_cmd_len = 1U;
}


/**
 *  1 byte
 * |      |
 * | 0x53 |
 * |      |
 */
void host_app::write_get_read_prot_cmd(void)
{
    tx_buffer[0] = 0x53;
    tx_cmd_len = 1U;
}

/**
 *  1 byte      4 bytes
 * |      |                 |
 * | 0x54 | jumping address |
 * |      |                 |
 */
void host_app::write_goto_addr_cmd(void)
{
    tx_buffer[0] = 0x54;
    int_or_chars jump_addr;
    cout << "What is the address you want to jump to ?" << endl;
    cin >> jump_addr.int_value;
    for (uint32_t i = 0U; i < 4U; i++) {
        tx_buffer[i + 1U] = jump_addr.ch_array[i];
    }
    tx_cmd_len = 5U;
}

void host_app::write_erase_flash_section_cmd(void)
{
    tx_buffer[0] = 0x55;
    tx_cmd_len = 1U;
}

void host_app::write_mem_read_cmd(void)
{
    tx_buffer[0] = 0x56;
    tx_cmd_len = 1U;
}

void host_app::write_mem_write_cmd(void)
{
    tx_buffer[0] = 0x57;
    tx_cmd_len = 1U;
}

void host_app::write_en_rw_prot_cmd(void)
{
    tx_buffer[0] = 0x58;
    tx_cmd_len = 1U;
}

void host_app::write_dis_rw_prot_cmd(void)
{
    tx_buffer[0] = 0x59;
    tx_cmd_len = 1U;
}

void host_app::write_read_sector_status_cmd(void)
{
    tx_buffer[0] = 0x5A;
    tx_cmd_len = 1U;
}

void host_app::write_read_otp_cmd(void)
{
    tx_buffer[0] = 0x5B;
    tx_cmd_len = 1U;
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