#!/bin/bash

# Destination IP address
IP="10.100.67.7"
TOS_HIGH_PRIORITY_UDP=0x10
TOS_MASK=0xff
IFACE=$(ip route get $IP | grep -oP "(?<=dev )[^ ]+")

sudo tc qdisc del dev $IFACE root
sudo tc qdisc replace dev $IFACE root handle 1: htb default 3
sudo tc class add dev $IFACE parent 1: classid 1:3 htb rate 3gbit
sudo tc qdisc add dev $IFACE parent 1:3 handle 30: pfifo_fast
sudo tc filter add dev $IFACE protocol ip parent 1: prio 0 u32 \
  match ip protocol 17 0xff \
  match ip tos $TOS_HIGH_PRIORITY_UDP $TOS_MASK \
  flowid 1:3
sudo tc filter add dev $IFACE protocol ip parent 1: prio 2 u32 \
  match ip protocol 6 0xff \
  flowid 1:3




IFACE_10GBIT="10Ge"  # Replace with your actual interface name
sudo tc qdisc del dev $IFACE_10GBIT root
sudo tc qdisc replace dev $IFACE_10GBIT root handle 1: cake bandwidth 10000mbit rtt 101ms diffserv4 ack-filter triple-isolate


sudo iptables -t mangle -A POSTROUTING -p udp -j DSCP --set-dscp-class CS6
sudo iptables -t mangle -A POSTROUTING -p tcp -j DSCP --set-dscp-class CS1
