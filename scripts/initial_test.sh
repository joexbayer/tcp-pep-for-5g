#!/bin/bash

SENDER="hylia"
RECEIVER="majora"
ROUTER="midna"

SENDER_IP="10.10.10.7"
SENDER_PATH="~/joe/uio-master-joeba"
RECEIVER_PATH="~/joe/uio-master-joeba"
ROUTER_DEV="enp24s0"

UDP_PACKETS=450
TRIES=1
flows=10

for (( try=1; try <= $TRIES; try++ )); do
    sudo echo "Test $try: $flows flows"

    ssh $ROUTER "sudo tc class change dev $ROUTER_DEV parent 1: classid 11 htb rate 15mbit" > /dev/null
    ssh $SENDER "cd $SENDER_PATH; python2 ultra_ping/echo.py --client $RECEIVER --n_packets $(($UDP_PACKETS+($flows*100)))" > /dev/null & 
    sleep 5

    for (( flow=1; flow <= $flows; flow++ )); do
        echo "Starting flow ${flow}"
        ssh $SENDER "cd $SENDER_PATH; ./client_file -p" > /dev/null &
    done

    sleep 13
    ssh $ROUTER "sudo tc class change dev $ROUTER_DEV parent 1: classid 11 htb rate 70mbit" > /dev/null
    wait

    ssh $SENDER "cd $SENDER_PATH; mv udp_packetn_latency_pairs results/udp_${flows}_flow/udp_${flows}_flows_${try}_pep_single" > /dev/null
done

ssh $RECEIVER "cd $RECEIVER_PATH; mv ./logs/server.log results/udp_${flows}_flow_pep.log" > /dev/null