#!/bin/bash
#
# Author:Joe Bayer
#
# Running a Step function test.
#
sudo echo "Starting";
ssh router "sudo tc class change dev enp24s0 parent 2: classid 10 htb rate 15mbit;sudo tc qdisc change dev enp24s0 parent 2:10 handle 11: bfifo limit 37500;";
ssh pc13 "cd uio-master-joeba; python2 ultra_ping/echo.py --client 172.16.11.5 --n_packets 600" > /dev/null & 
sudo echo "Ultra ping";
sleep 5;
ssh pc04 "cd uio-master-joeba; ./client_file" &
sudo echo "TCP started";
sleep 13;
ssh router "sudo tc qdisc change dev enp24s0 parent 2:10 handle 11: bfifo limit  175000;sudo tc class change dev enp24s0 parent 2: classid 10 htb rate 70mbit;";
wait
ssh pc13 "cd ~/uio-master-joeba;mv udp_packetn_latency_pairs results/udp_bifo_transfer" > /dev/null



UDP_PACKETS=550
TRIES=1
#for (( flows=1; flows <= $TRIES; flows++ )); do
flows=6
for (( try=1; try <= $TRIES; try++ )); do
    sudo echo "Test $try: $flows flows";
    ssh router "sudo tc class change dev enp24s0 parent 1: classid 11 htb rate 15mbit" > /dev/null;
    ssh pc13 "cd uio-master-joeba; python2 ultra_ping/echo.py --client 172.16.11.5 --n_packets $(($UDP_PACKETS*3))" > /dev/null & 
    sleep 5;

    for (( flow=1; flow <= $flows; flow++ )); do
        echo "Starting flow ${flow}"
        ssh pc04 "cd uio-master-joeba; ./client_file -p" > /dev/null &
    done

    sleep 10;
    #ssh router "sudo tc class change dev enp24s0 parent 1: classid 11 htb rate 30mbit" > /dev/null;
    wait
    ssh pc13 "cd ~/uio-master-joeba;mv udp_packetn_latency_pairs results/udp_${flows}_flows_${try}_pep" > /dev/null
done
#done

# FQ_CODEL
ssh router "sudo tc class change dev enp24s0 parent 1: classid 11 htb rate 15mbit" > /dev/null;
ssh pc04 "cd uio-master-joeba; python2 ultra_ping/echo.py --client 172.16.11.5 --n_packets 600" > /dev/null & 
sleep 5;
ssh pc04 "cd uio-master-joeba; ./client_file -p" > /dev/null &
sleep 13;
ssh router "sudo tc class change dev enp24s0 parent 1: classid 11 htb rate 70mbit" > /dev/null;
wait
ssh pc04 "cd ~/uio-master-joeba;mv udp_packetn_latency_pairs results/udp_pep_transfer" > /dev/null

