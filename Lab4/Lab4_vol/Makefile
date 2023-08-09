export
CROSS_  = riscv64-unknown-elf-
AR      = ${CROSS_}ar
CC      = $(CROSS_)gcc
LD      = $(CROSS_)ld
OBJCOPY = $(CROSS_)objcopy


TYPE    ?= Debug

ifeq ($(TYPE), Debug)
	TYPEFLAG = -g
else
	TYPEFLAG = -O3
endif

ISA     ?= rv64imafd
ABI     ?= lp64
INCLUDE  = -I$(shell pwd)/include -I$(shell pwd)/arch/riscv/include
CF       = $(TYPEFLAG) -march=$(ISA) -mabi=$(ABI) -mcmodel=medany -ffunction-sections -fdata-sections -nostartfiles -nostdlib -nostdinc -static -lgcc -Wl,--nmagic -Wl,--gc-sections
CFLAG    = $(CF) $(INCLUDE) -DPRIORITY

.PHONY: all run debug clean
all:
	@make -C init -s
	@make -C lib -s
	@make -C arch/riscv -s
	@echo "\033[32mMake vmlinux Success! \033[0m"

run: all
	@qemu-system-riscv64 -nographic -machine virt -kernel vmlinux

debug: all
	@qemu-system-riscv64 -nographic -machine virt -kernel vmlinux -S -s

clean:
	@make clean -C init -s
	@make clean -C lib -s
	@make clean -C arch/riscv -s
	@echo "\033[32mMake clean Success! \033[0m"
