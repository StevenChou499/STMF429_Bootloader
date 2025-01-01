// serial_posix.cpp
#include "host_app.hpp"
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <stdint.h>
// #include <conio.h>
using namespace std;

int main()
{
    string portname = "/dev/ttyACM0";
    host_app btldr_app(portname);

    // cout << "host_app created..." << endl;

    while (1) {
        btldr_app.show_prompt();
        if (btldr_app.parse_command() == false)
            break;
        btldr_app.get_bootloader_respond();
        cout << "Please press enter to continue...";
        cin.get();
        cin.get();
    }
    
    cout << "host_app exiting..." << endl;
    sleep(1);

    return 0;
}
