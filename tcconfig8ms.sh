sudo tc qdisc del dev enp0s8 root;
sudo tc qdisc add dev enp0s8 root handle 2: netem delay 8ms;
sudo tc qdisc add dev enp0s8 parent 2: handle 3: htb default 10;
sudo tc class add dev enp0s8 parent 3: classid 10 htb rate 1000Mbit;
sudo tc qdisc add dev enp0s8 parent 3:10 handle 11: bfifo limit 1000000;

#ssleep 4
#sudo tc qdisc change dev enp0s9 parent 3:10 handle 11: bfifo limit 250000;