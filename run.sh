#!/bin/bash

TIME=$1
BLOCK=$2
CPUS=$3

PIDS=()
for ((i=0; i<${CPUS}; i++)); do
	if [ $i -eq 0 ]; then
		taskset -c $i "./mbw_$BLOCK" &
	else
		taskset -c $i "./mbw_$BLOCK" 1>/dev/null &
	fi
	PIDS+=($!)
done

sleep $TIME

for pid in "${PIDS[@]}"
do
	kill ${pid}
done
