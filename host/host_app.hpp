#include <string>
#include <cstdint>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <termios.h>

using std::string, std::cout, std::cerr, std::hex, std::endl;

class host_app {
    public:
        int32_t mcu_fd;
        uint32_t get_crc(uint8_t *pBuf, uint32_t length);
        int32_t users_choice;
        
    public:
        host_app(string portname);
        ~host_app();
        void show_prompt(void);
        void get_user_input(void);
};

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

void host_app::get_user_input(void)
{
    string option;
    cin >> option;
}