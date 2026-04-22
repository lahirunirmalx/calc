# Build all calculator variants: GTK+3 (calc, calc2) and SDL2 (calc_sdl).

CC = gcc
CFLAGS = -Wall -Wextra -O2
CPPFLAGS =

GTK_PKG = gtk+-3.0
GTK_CFLAGS := $(shell pkg-config --cflags $(GTK_PKG))
GTK_LIBS := $(shell pkg-config --libs $(GTK_PKG))

SDL_PKGS = sdl2 SDL2_ttf
SDL_CFLAGS := $(shell pkg-config --cflags $(SDL_PKGS))
SDL_LIBS := $(shell pkg-config --libs $(SDL_PKGS))

TARGETS = calc calc2 calc_sdl
CORE = calc_core.c

all: $(TARGETS)

calc: calc.c $(CORE)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(GTK_CFLAGS) -o $@ calc.c $(CORE) $(GTK_LIBS) -lm

calc2: calc2.c $(CORE)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(GTK_CFLAGS) -o $@ calc2.c $(CORE) $(GTK_LIBS) -lm

calc_sdl: calc_sdl.c $(CORE)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SDL_CFLAGS) -o $@ calc_sdl.c $(CORE) $(SDL_LIBS) -lm

clean:
	rm -f $(TARGETS)

.PHONY: all clean
