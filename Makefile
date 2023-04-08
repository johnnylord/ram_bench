CC = gcc
CFLAGS = -lm
OUTPUT = bin

all: \
	benchmark_random_access_block_4096 \
	benchmark_random_access_block_64 \
	mbw_random_access_block_4096 \
	mbw_sequential_access_block_4096

benchmark_random_access_block_4096: benchmark.c
	$(CC) -D BLOCK_SIZE=4096 -D RANDOM_MODE=1 -o $(OUTPUT)/$@ $< $(CFLAGS)

benchmark_random_access_block_64: benchmark.c
	$(CC) -D BLOCK_SIZE=64 -D RANDOM_MODE=1 -o $(OUTPUT)/$@ $< $(CFLAGS)

mbw_random_access_block_4096: mbw.c
	$(CC) -D BLOCK_SIZE=4096 -D RANDOM_MODE=1 -o $(OUTPUT)/$@ $< $(CFLAGS)

mbw_sequential_access_block_4096: mbw.c
	$(CC) -D BLOCK_SIZE=4096 -D RANDOM_MODE=0 -o $(OUTPUT)/$@ $< $(CFLAGS)

clean:
	rm -rf *.png
	rm -rf $(OUTPUT)/*
