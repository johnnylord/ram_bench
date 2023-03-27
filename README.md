RAM Bench
=========

Usage of benchmark
-----
Use this tool to understand the read write latency based on different memory block size and memory footprint size (working set size)

1. Compile
```bash
make
```
2. Read benchmark
```bash
./benchmark_4096 --mode 1 --size 2

# Other options
./benchmark_1024 --mode 1 --size 2
./benchmark_256 --mode 1 --size 2
./benchmark_64 --mode 1 --size 2
```
3. Write benchmark (Including read also)
```bash
./benchmark_4096 --mode 2 --size 2

# Other options
./benchmark_1024 --mode 2 --size 2
./benchmark_256 --mode 2 --size 2
./benchmark_64 --mode 2 --size 2
```

## Usage of bwtable
Use this tool to find the proper memory block size and memory footprint size (working set size) given the max data bandwidth.

1. Compile
```bash
make
```
2. Write bandwidth benchmark (Single Core)
```bash
./bwtable_4096 --size 2 --begin 64 --end 2048

# Other options
./bwtable_1024 --size 2 --begin 64 --end 2048
./bwtable_256 --size 2 --begin 64 --end 2048
./bwtable_64 --size 2 --begin 64 --end 2048
```

## Usage of mbw
Use this tool to generate different memory workload based on wokring set size and block size.

1. Compile
```bash
make
```
2. Memory workload benchmark (Single Core)
```bash
./mbw_4096 --size 2 --work 1024

# Other options
./mbw_1024 --mode 2 --work 1024
./mbw_256 --mode 2 --work 1024
./mbw_64 --mode 2 --work 1024
```


## Simulate the memory workload with multiple cores
```
# Run for 30 seconds
# Memory block size 4096
# Number of duplicate tasks 8 (on 8 cores)
./run.sh 30 4096 8
```

## NOTES:
- Orin-NX 16GB
```bash
# 49% memory utilization with EMC=3199Mhz
./run.sh 30 4096 8

# 25% memory utilization with EMC=3199Mhz
./run.sh 30 4096 4

# 49% memory utilization with EMC=2133Mhz
./run.sh 30 4096 6

# 24% memory utilization with EMC=2133Mhz
./run.sh 30 4096 2
```

