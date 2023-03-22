CC = gcc
CFLAGS = -O3 -lm

all: benchmark

benchmark: benchmark.c
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm -rf *.png
	rm -rf benchmark
