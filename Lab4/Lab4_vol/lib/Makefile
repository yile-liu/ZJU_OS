.PHONY: all clean
all: put.o rand.o

put.o: put.c
	$(CC) -c put.c $(CFLAG)
rand.o: rand.c
	$(CC) -c rand.c $(CFLAG)

clean:
	rm -f *.o
