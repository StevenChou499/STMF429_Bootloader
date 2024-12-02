include toolchain.mk

BL_BIN    ?= bootloader
HOST_BIN  ?= host_app

BUILD_DIR ?= $(CURDIR)/build
HOST_DIR  ?= $(CURDIR)/host

BL_TARGET = $(BUILD_DIR)/$(BL_BIN)
HOST_TARGET = $(HOST_DIR)/$(HOST_BIN)

all: $(BL_TARGET) $(HOST_TARGET)

btldr: $(BL_TARGET)

host: $(HOST_TARGET)

$(BL_TARGET):
	@$(MAKE) -C bootloader OUT=$(BUILD_DIR) BIN=$(BL_BIN)

$(HOST_TARGET):
	@$(MAKE) -C host OUT=$(HOST_DIR) BIN=$(HOST_BIN)

debug:
	$(DB) $(BL_TARGET)
	openocd -f board/st_nucleo_f4.cfg

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) $(HOST_TARGET).o
	$(RM) $(HOST_TARGET)

.PHONY: all $(BL_TARGET) debug clean	