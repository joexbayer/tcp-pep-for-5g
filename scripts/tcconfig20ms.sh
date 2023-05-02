sudo tc qdisc del dev enp0s9 root;
sudo tc qdisc add dev enp0s9 root handle 2: netem delay 20ms;
sudo tc qdisc add dev enp0s9 parent 2: handle 3: htb default 10;
sudo tc class add dev enp0s9 parent 3: classid 10 htb rate 1Mbit;
sudo tc qdisc add dev enp0s9 parent 3:10 handle 11: bfifo limit 2500;