#!/bin/bash

DB="$(uname -n)/cpu.rrd"
CACHED="sullivan:42217"

names=("user" "system" "idle")
for ((i=0; i<$(nproc); i++ )); do
  names+=("user$i" "system$i" "idle$i")
done
DSs=()
for name in "${names[@]}"; do
  DSs+=("DS:$name:COUNTER:1m:0:60000")
done
rrdtool create $DB -d $CACHED -b now -s 1m ${DSs[@]} \
  RRA:AVERAGE:0.5:1m:1d \
  RRA:AVERAGE:0.5:1h:2M \
  RRA:AVERAGE:0.5:1d:5yr

