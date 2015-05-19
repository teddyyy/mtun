CC = gcc

objects = mtun.o socket.o handle.o tap.o monitor.o bridge.o
CFLAGS = -Wall -g
LDFLAGS = -lpthread

mtun : $(objects)
	$(CC) -o mtun $(objects) $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c $*.c

.PHONY : clean
clean :
	rm mtun $(objects)
