
CFLAGS := -Wall -Wextra -std=c11
CFLAGS += $(shell pkg-config sdl3 sdl3-ttf --cflags)

LDFLAGS := $(shell pkg-config sdl3 sdl3-ttf --libs)


td: main.c
	$(CC) $(CFLAGS) main.c -o td $(LDFLAGS)
