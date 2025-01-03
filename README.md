# STMF429_Bootloader

This is a bootloader specifically for STMF429xx boards.

## Toolchain installation

### Install Arm GNU Toolchain: 

Some required packages before installing: 

```
sudo apt-get install libncursesw5
```

At [Arm GNU Toolchain Downloads](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) website

After installing, you can check the installation by entering: 

```
arm-none-eabi-gcc --version
```

### Install OpenOCD: 

Cloning OpenOCD from GitHub

```
git clone git://git.code/sf/net/p/openocd/code openocd
```

Install required libraries

```
sudo apt-get install build-essential pkg-config autoconf automake libtool libusb-dev libusb-1.0-0-dev libhidapi-dev libsysfs-dev
```

Head into the cloned folder and build up the environment: 

```
cd openocd/
sudo ./bootstrap
sudo ./configure
```

Last, compile and install:

```
sudo make
sudo make install
```

After installing, you can check the installation by entering: 

```
openocd -v
```

## Build options

Compile the whole project:
```
make all
```

or 

```
make
```

Directly load the bootloader into the board:

```
make load
```

Use arm-none-eabi-gdb to debug the program:
```
make debug
```

Remove all the built binaries:
```
make clean
```

## Memory Layout

The bootloader starts at address `0x08000000` , and the default loading address for user application sits at address `0x08008000`.

```
 0x08000000 -+-------------+
             |             |
             | Boot-loader |
             |             |
 0x08008000  +-------------+
             |             |
             |    User     |
             | Application |
             |             |
             +-------------+
```

## Bootloader Command Options

The bootloader gives the user options to program/read/write the flash memory, disable or enable R/W protection or reading OTP area.

```
Bootloader command options: 
Get bootloader version                :  1
Get bootloader help message           :  2
Get bootloader chip id                :  3
Get bootloader read protection status :  4
Go to specific address                :  5
Erase flash sections                  :  6
Memory read                           :  7
Memory write                          :  8
Enable read write protection          :  9
Disable read write protection         : 10
Read sector status                    : 11
Read one time programmable memory     : 12
quit                                  :  q
```