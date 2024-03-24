#!/bin/bash

# Interface name (adjust as necessary)
IFACE="eth0"
tc qdisc del dev $IFACE root
tc qdisc add dev $IFACE root handle 1: htb default 10
tc class add dev $IFACE parent 1: classid 1:10 htb rate 3gbit
tc qdisc add dev $IFACE parent 1:10 handle 10: pfifo_fast
tc filter add dev $IFACE protocol ip parent 1: prio 1 u32 match ip protocol 17 0xff flowid 1:10

# Script end
