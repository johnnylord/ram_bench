RAM Bench
=========

## Compile
```
make
```

## Simulate the memory workload with multiple cores
```
# Run for 30 seconds
# Memory block size 4096
# Access mdoe: random / sequential
# Number of duplicate tasks 8 (on 8 cores)
./run.sh 30 4096 random 8
```
