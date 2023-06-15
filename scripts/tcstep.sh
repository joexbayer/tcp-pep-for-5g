#!/bin/bash
#
# Author:Joe Bayer
#
# Running a Step function test.
#
sudo echo "Starting";
ssh router "sudo tc class change dev enp24s0 parent 2: classid 10 htb rate 15mbit;sudo tc qdisc change dev enp24s0 parent 2:10 handle 11: bfifo limit 37500;";
ssh pc13 "time make -C ~/uio-master-joeba/ ultra_client" &
sudo echo "Ultra ping";
sleep 5;
ssh pc04 "cd uio-master-joeba; ./client_file" &
sudo echo "TCP started";
sleep 10;
ssh router "sudo tc qdisc change dev enp24s0 parent 2:10 handle 11: bfifo limit 75000;sudo tc class change dev enp24s0 parent 2: classid 10 htb rate 30mbit;";
sudo echo "Done";


# FQ_CODEL
sudo echo "Starting";
ssh router "sudo tc class change dev enp24s0 parent 1: classid 11 htb rate 15mbit";
ssh pc04 "time make -C ~/uio-master-joeba/ ultra_client" &
sudo echo "Ultra ping";
sleep 5;
ssh pc04 "cd uio-master-joeba; ./client_file -p" &
sudo echo "TCP started";
sleep 10;
ssh router "sudo tc class change dev enp24s0 parent 1: classid 11 htb rate 30mbit";
sudo echo "Done";
