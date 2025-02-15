#!/bin/bash

DB="$(uname -n)/gpu.rrd"
CACHED="sullivan:42217"
GPUS=$(nvidia-smi -L | wc -l)

DSs=()
for ((i = 0; i < GPUS; i++)); do
  DSs+=("DS:util$i:GAUGE:1m:0:100" "DS:mem$i:GAUGE:1m:0:U" "DS:power$i:GAUGE:1m:0:U")
done

rrdtool create $DB -d $CACHED -b now -s 1m ${DSs[@]} \
  RRA:AVERAGE:0.5:1m:1d \
  RRA:AVERAGE:0.5:1h:2M \
  RRA:AVERAGE:0.5:1d:5yr

