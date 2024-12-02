include ../toolchain.mk

C_SRC = bootloader.c \
        crc.c \
	    gpio.c \
	    main.c \
	    myprintf.c \
	    uart.c

C_INC = -I/inc/

A_SRC = startup.S

LD = bootloader.ld

C_FLAGS = -mthumb \
          -mcpu=cortex-m4 \
		  -Wall \
		  -g

LD_FLAGS = -mcpu=cortex-m4 \
           -nostdlib \
		   -T $(LD)

OBJS  = $(patsubst %.c, $(OUT)/%.o, $(notdir $(C_SRC)))
OBJS += $(patsubst %.S, $(OUT)/%.o, $(notdir $(A_SRC)))

all: $(OUT)/$(BIN)

$(OUT)/%.o: $(CURDIR)/src/%.c
	@$(CC) -c $(C_FLAGS) $(C_INC) $< -o $@

$(OUT)/%.o: $(CURDIR)/src/%.S
	@$(CC) -c $(C_FLAGS) $< -o $@

$(OUT)/$(BIN): $(OUT) $(OBJS)
	@$(CC) $(OBJS) $(LD_FLAGS) -o $@
	@$(SZ) $@

$(OUT):
	@mkdir $@