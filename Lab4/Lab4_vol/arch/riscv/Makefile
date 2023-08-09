# makefile_riscv
.PHONY: all clean
all: kernel/vmlinux.lds
	make -C kernel
	$(LD) -o ../../vmlinux ../../init/*.o ../../lib/*.o kernel/*.o -T kernel/vmlinux.lds
	$(OBJCOPY) -O binary ../../vmlinux boot/Image --strip-all
	nm ../../vmlinux > ../../System.map

clean:
	make clean -C kernel
	rm -f ../../vmlinux ../../System.map boot/Image 
