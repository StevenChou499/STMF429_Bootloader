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
    public:
        host_app(string portname);
        ~host_app();
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

}

host_app::~host_app()
{
    if (mcu_fd >= 0)
        close(mcu_fd);
}