CC = gcc
CFLAGS = -Wall -std=c99 -g
LFLAGS = -lSDL2

LIBS = bitmap tar wav
OUT = $(LIBS:%=%.bin)

.PHONY: all
.SECONDARY: $(LIBS)

build:
	@echo "Testando"

all: $(OUT)

bitmap.bin: examples/bitmap.c bitmap/bitmap.h
	$(CC) $< -o $@ $(CFLAGS) $(LFLAGS) -Ibitmap

clean:
	rm -f $(OUT)