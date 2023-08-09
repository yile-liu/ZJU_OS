# makefile_kernel
.PHONY: all clean
all: sched.o head.o entry.o strap.o vm.o buddy.o slub.o

%.o: %.S
	$(CC) -c $< $(CFLAG)

%.o: %.c
	$(CC) -c $< $(CFLAG)

clean:
	rm -f *.o
