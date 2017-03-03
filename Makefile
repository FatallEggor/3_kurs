CC = gcc
CFLAGS_ALL = -Wall -o
CFLAGS_OBJS = -Wall -c
OBJS = gf.o kuzn.o main.o

.PHONY: all clean


all: $(OBJS)
	gcc $(CFLAGS_ALL) kuzn_chiper $(OBJS)
	
gf.o: gf.c
	$(CC) $(CFLAGS_OBJS) gf.c

kuzn.o: kuzn.c
	$(CC) $(CFLAGS_OBJS) kuzn.c
main.o: main.c
	$(CC) $(CFLAGS_OBJS) main.c



clean:
	rm -f $(OBJS) kuzn_chiper

	

