#!/bin/bash
#
# Author: Kr1stj0n C1k0 (kristjoc@ifi.uio.no)
#
# log TCP cwnd stats
#

# Poll interval in seconds
INTERVAL='0.0001'
# Log file
LOG_FILE=$1
# Number of flows
NFLOWS=$2
# Source IP
SRC_IP=$3
# First Source Port
START_TCP_SPORT=$4
# Destination IP
DST_IP=$5
# Destination Port
DST_PORT=$6

while true ; do
  BEFORE=$(date +%s.%N)

  start_sport=$START_TCP_SPORT
  end_sport=$((start_sport + $NFLOWS - 1))

  for (( sport=$start_sport; sport<=$end_sport; sport++ )); do
    output=$(sudo ss -i '( sport = :'${sport}' )' dst $DST_IP src $SRC_IP)
    cwnd=$(echo "$output" | grep -Po 'cwnd:\K.[0-9]*')
    if [ -z "$cwnd" ]; then
      cwnd="0"
    fi
    echo "$BEFORE,$SRC_IP,$sport,$DST_IP,$DST_PORT,$cwnd" >> $LOG_FILE
  done
done
