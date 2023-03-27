#!/bin/bash
#
# Author: Kr1stj0n C1k0 (kristjoc@ifi.uio.no)
#
# log TCP cwnd stats
#

# Poll interval in seconds
INTERVAL='0.0001'
# Log file
LOG_FILE="tcp-cwnd.dat"
# Source IP
SRC_IP="192.168.1.11"
# Destination IP
DST_IP="192.168.2.22"
# Destination Port
DST_PORT="5001"
SRC_PORT="33333"
while true ; do
  BEFORE=$(date +%s.%N)

  output=$(sudo ss -i '( dport == 5201 )' dst $DST_IP src $SRC_IP)
  cwnd=$(echo "$output" | grep -Po 'cwnd:\K.[0-9]*')
  if [ -z "$cwnd" ]; then
    cwnd="0"
  fi
  echo "$BEFORE,$SRC_IP,$SRC_PORT,$DST_IP,$DST_PORT,$cwnd" >> $LOG_FILE
done
# AFTER=`date +%s.%N`
# SLEEP_TIME=`echo $BEFORE $AFTER $INTERVAL \
  # | awk '{ st = $3 - ($2 - $1) ; if ( st < 0 ) st = 0 ; print st }'`
# sleep $SLEEP_TIME
# done
