#!/bin/bash

TIME=$1
SIZE=$2
MODE=$3
CPUS=$4

PIDS=()
for ((i=0; i<${CPUS}; i++)); do
        if [ $i -eq 0 ]; then
                taskset -c $i "./bin/mbw_${MODE}_access_block_${SIZE}" &
        else
                taskset -c $i "./bin/mbw_${MODE}_access_block_${SIZE}" 1>/dev/null &
        fi
        PIDS+=($!)
done

sleep $TIME

for pid in "${PIDS[@]}"
do
        kill ${pid} >/dev/null 2>&1
done
