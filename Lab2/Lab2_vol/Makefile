# export 关键字定义的变量在调用子 Makefile 文件时依然有效
export
CROSS_  = riscv64-unknown-elf-
CC      = $(CROSS_)gcc
LD      = $(CROSS_)ld
OBJCOPY = $(CROSS_)objcopy

DEBUG  ?= true

# gcc 编译相关参数
ISA     = rv64imafd
ABI     = lp64
INCLUDE = -I$(shell pwd)/include -I$(shell pwd)/arch/riscv/include
CF      = -g -march=$(ISA) -mabi=$(ABI) -mcmodel=medany -ffunction-sections -fdata-sections -nostartfiles -nostdlib -nostdinc -fno-builtin -static -lgcc 
CFLAG   = ${CF} ${INCLUDE} ${TASK_MM}

ifneq ($(DEBUG), )  
  CFLAG += -DDEBUG_LOG
endif

all: vmlinux

.PHONY: vmlinux run debug clean
vmlinux:
	$(MAKE) -C arch/riscv all 
	$(LD) -T arch/riscv/kernel/vmlinux.lds arch/riscv/kernel/*.o -o vmlinux
	$(shell test -d arch/riscv/boot || mkdir -p arch/riscv/boot)
	$(OBJCOPY) -O binary vmlinux arch/riscv/boot/Image
	nm vmlinux > System.map
	@echo "\033[32mMake vmlinux Success! \033[0m"

run: vmlinux
	@qemu-system-riscv64 -nographic --machine virt -bios default -device loader,file=vmlinux,addr=0x80200000 -D log

debug: vmlinux
	@qemu-system-riscv64 -nographic -machine virt -bios default -device loader,file=vmlinux,addr=0x80200000 -S -s

clean:
	$(MAKE) -C arch/riscv clean
	$(shell test -f vmlinux && rm vmlinux)
	$(shell test -f System.map && rm System.map)
	@echo "\033[32mMake clean Success! \033[0m"

