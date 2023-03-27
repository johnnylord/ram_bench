CC = gcc
CFLAGS = -O3 -lm

all: \
	benchmark_4096 benchmark_1024 benchmark_256 benchmark_64 \
	bwtable_4096 bwtable_1024 bwtable_256 bwtable_64 \
	mbw_4096 mbw_1024 mbw_256 mbw_64

benchmark_4096: benchmark.c
	$(CC) -o $@ $< $(CFLAGS)

benchmark_1024: benchmark.c
	$(CC) -D BLOCK_SIZE=1024 -o $@ $< $(CFLAGS)

benchmark_256: benchmark.c
	$(CC) -D BLOCK_SIZE=256 -o $@ $< $(CFLAGS)

benchmark_64: benchmark.c
	$(CC) -D BLOCK_SIZE=64 -o $@ $< $(CFLAGS)

bwtable_4096: bwtable.c
	$(CC) -o $@ $< $(CFLAGS)

bwtable_1024: bwtable.c
	$(CC) -D BLOCK_SIZE=1024 -o $@ $< $(CFLAGS)

bwtable_256: bwtable.c
	$(CC) -D BLOCK_SIZE=256 -o $@ $< $(CFLAGS)

bwtable_64: bwtable.c
	$(CC) -D BLOCK_SIZE=64 -o $@ $< $(CFLAGS)

mbw_4096: mbw.c
	$(CC) -o $@ $< $(CFLAGS)

mbw_1024: mbw.c
	$(CC) -D BLOCK_SIZE=1024 -o $@ $< $(CFLAGS)

mbw_256: mbw.c
	$(CC) -D BLOCK_SIZE=256 -o $@ $< $(CFLAGS)

mbw_64: mbw.c
	$(CC) -D BLOCK_SIZE=64 -o $@ $< $(CFLAGS)

clean:
	rm -rf *.png
	rm -rf benchmark_*
	rm -rf bwtable_*
	rm -rf mbw_*
