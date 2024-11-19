// serial_posix.cpp
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <stdint.h>
using namespace std;

uint32_t get_crc(uint8_t *pBuf, uint32_t length)
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

int main()
{
    // Replace with your serial port name
    const char* portname = "/dev/ttyACM0";

    int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        cerr << "Error opening " << portname << ": " << strerror(errno) << endl;
        return 1;
    }

    struct termios tty;
    tcgetattr(fd, &tty);
    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;     // set data bits as 8 bit
    tty.c_cflag &= ~(CSTOPB | PARENB | PARODD | CRTSCTS);
    tty.c_cflag |= (CREAD | CLOCAL);

    tty.c_iflag &= ~(IGNBRK | IXON | IXOFF | IXANY | INLCR | IGNCR | ICRNL);
    tty.c_oflag = 0;
    tty.c_lflag = 0;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        cerr << "Error from tcsetattr: " << strerror(errno) << endl;
        return 1;
    }

    uint8_t cmd_len = 1;
    write(fd, &cmd_len, 1);

    uint8_t cmd_code[128];
    cmd_code[0] = 0x51;
    uint32_t crc = get_crc(cmd_code, 1);
    *(uint32_t *)(cmd_code + 1) = crc;
    write(fd, cmd_code, 5);

    uint8_t respond[1024] = {0};
    read(fd, respond, 1);

    cout << "The responded code is " << (uint32_t)respond[0] << endl;

    if (respond[0] == (uint8_t) 0xA) {
        // read(fd, respond + 1, 1);
        uint32_t read_length = 572U;
        // read(fd, respond + 1, read_length);
        uint32_t total_read_bytes = 0U;
        while (total_read_bytes < read_length) {
            int bytesRead = read(fd, respond + 1 + total_read_bytes, read_length - total_read_bytes);
            total_read_bytes += bytesRead;
            cout << "Read " << bytesRead << " bytes" << endl;
            cout << "Total read " << total_read_bytes << " bytes" << endl;
        }
        // uint32_t length = read(fd, respond + 1, 14);
        // cout << "The bootloader version is " << (uint32_t)respond[1] << endl;
        // cout << "Bootloader help message : " << endl << respond[0] << endl;
        // for (int i = 1; i <= 572; i++) {
            // cout << (uint32_t) respond[i] << ", (" << respond[i] << ")" << endl;
        // }
        cout << "Total read " << total_read_bytes << " bytes" << endl;
        cout << "Bootloader help message: " << endl << respond + 1 << endl;
    } else if (respond[0] == (uint8_t) 0xC) {
        cerr << "Error cmd from bootloader" << endl;
    }

    // cout << "The value of index 0 and 1 is " << (uint32_t)respond[0] << ", " << (uint32_t)respond[1] << endl;

    return 0;
}

// This code is contributed by Susobhan Akhuli
