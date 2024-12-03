// serial_posix.cpp
#include "host_app.hpp"
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <stdint.h>
using namespace std;

int main()
{
    string portname = "/dev/ttyACM0";
    host_app btldr_app(portname);

    // cout << "host_app created..." << endl;

    while (1) {
        btldr_app.show_prompt();
        btldr_app.parse_command();

        // uint8_t cmd_len = 1;
        // write(btldr_app.mcu_fd, &cmd_len, 1);

        // uint8_t cmd_code[128];
        // cmd_code[0] = 0x52;
        // uint32_t crc = btldr_app.get_crc(cmd_code, 1);
        // *(uint32_t *)(cmd_code + 1) = crc;
        // write(btldr_app.mcu_fd, cmd_code, 5);

        // uint8_t respond[1024] = {0};
        // read(btldr_app.mcu_fd, respond, 1);

        // cout << "The responded code is " << (uint32_t)respond[0] << endl;

        // if (respond[0] == (uint8_t) 0xA) {
        //     // read(fd, respond + 1, 1);
        //     uint32_t read_length = 4U;
        //     // read(fd, respond + 1, read_length);
        //     uint32_t total_read_bytes = 0U;
        //     while (total_read_bytes < read_length) {
        //         int bytesRead = read(btldr_app.mcu_fd, respond + 1 + total_read_bytes, read_length - total_read_bytes);
        //         total_read_bytes += bytesRead;
        //         cout << "Read " << bytesRead << " bytes" << endl;
        //         cout << "Total read " << total_read_bytes << " bytes" << endl;
        //     }
        //     // uint32_t length = read(fd, respond + 1, 14);
        //     // cout << "The bootloader version is " << (uint32_t)respond[1] << endl;
        //     // cout << "Bootloader help message : " << endl << respond[0] << endl;
        //     // for (int i = 1; i <= 572; i++) {
        //         // cout << (uint32_t) respond[i] << ", (" << respond[i] << ")" << endl;
        //     // }
        //     // cout << "Total read " << total_read_bytes << " bytes" << endl;
        //     // cout << "Bootloader help message: " << endl << respond + 1 << endl;
        //     cout << "Micro-controller's ID value : " << hex << *(uint32_t *)(respond + 1) << endl;
        // } else if (respond[0] == (uint8_t) 0xC) {
        //     cerr << "Error cmd from bootloader" << endl;
        // }

        // cout << "The value of index 0 and 1 is " << (uint32_t)respond[0] << ", " << (uint32_t)respond[1] << endl;
    }
    
    cout << "host_app exiting..." << endl;
    sleep(1);

    return 0;
}
