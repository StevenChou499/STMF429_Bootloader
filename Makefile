include toolchain.mk

print:
	$(MAKE) -C ./bootloader what
	echo Current directory is $(CURDIR)