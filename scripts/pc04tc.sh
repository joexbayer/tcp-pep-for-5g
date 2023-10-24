#!/bin/bash
#
# Author: Joe Bayer (joeba@ifi.uio.no)
#
# Setup for the client traffic control.
# Note: Delay is configured in the router.
#


# using fq with 50mbit
sudo tc qdisc del dev enp36s0 root;
sudo tc qdisc add dev enp36s0 root fq maxrate 50mbit;
# sudo ip link set dev enp36s0 gso_max_size 1500;

# FIFO (bfifo) 100mbit with 25000 packet queue.
sudo tc qdisc del dev enp36s0 root;
sudo tc qdisc add dev enp36s0 root handle 2: htb default 10;
sudo tc class add dev enp36s0 parent 2: classid 10 htb rate 100Mbit;
sudo tc qdisc add dev enp36s0 parent 2:10 handle 11: bfifo limit 25000;

# fq_codel 100mbit with 150ms interval
sudo tc qdisc del dev enp36s0 root;
sudo tc qdisc add dev enp36s0 root handle 1: htb default 11;
sudo tc class add dev enp36s0 parent 1: classid 11 htb rate 100mbit;
sudo tc qdisc add dev enp36s0 parent 1:11 fq_codel interval 150;


# IPERF
bash tcpss2.sh flow1.dat 3 172.16.10.4 33333 172.16.11.5 5001 &
iperf -c pc05 -B 172.16.10.4:33333 & iperf -c pc05 -B 172.16.10.4:33334 & iperf -c pc05 -B 172.16.10.4:33335;