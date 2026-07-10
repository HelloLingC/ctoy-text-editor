
CFLAGS := -Wall -Wextra -std=c11
CFLAGS += $(shell pkg-config sdl3 sdl3-ttf --cflags)

LDFLAGS := $(shell pkg-config sdl3 sdl3-ttf --libs)


OBJS = main.o button.o document.o

td: $(OBJS)
	$(CC) $(OBJS) -o td $(LDFLAGS)

main.o: main.c button.h document.h
	$(CC) $(CFLAGS) -c main.c

button.o: button.c button.h
	$(CC) $(CFLAGS) -c button.c

document.o: document.c document.h
	$(CC) $(CFLAGS) -c document.c