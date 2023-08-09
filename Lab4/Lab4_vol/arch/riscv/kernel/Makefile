# makefile_kernel
.PHONY: all clean
all: sched.o head.o entry.o strap.o vm.o

head.o: head.S
	$(CC) -c head.S $(CFLAG)

entry.o: entry.S
	$(CC) -c entry.S $(CFLAG)

strap.o: strap.c
	$(CC) -c strap.c $(CFLAG)

sched.o: sched.c
	$(CC) -c sched.c $(CFLAG)

vm.o : vm.c
	$(CC) -c vm.c $(CFLAG)

clean:
	rm -f *.o
