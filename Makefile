CC = gcc
CFLAGS = -Wall -std=c99 -g
LFLAGS = -lSDL2

LIBS = bitmap tar wav

.PHONY: all buld

build:
	@echo "Testando"

all: $(LIBS)

bitmap: examples/bitmap.c bitmap.h
	$(CC) $< -o $@ $(CFLAGS) $(LFLAGS)

wav: examples/wav.c wav.h
	$(CC) $< -o $@ $(CFLAGS) $(LFLAGS)

tar: examples/tar.c tar.h
	$(CC) $< -o $@ $(CFLAGS) $(LFLAGS)

clean:
	rm -f $(LIBS)