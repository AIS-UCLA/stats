#!/bin/bash

DB="$(uname -n)/mem.rrd"
CACHED="sullivan:42217"

names=("total" "free" "avail")
DSs=()
for name in "${names[@]}"; do
  DSs+=("DS:$name:GAUGE:1m:0:U")
done
rrdtool create $DB -d $CACHED -b now -s 1m ${DSs[@]} \
  RRA:AVERAGE:0.5:1m:1d \
  RRA:AVERAGE:0.5:1h:2M \
  RRA:AVERAGE:0.5:1d:5yr

