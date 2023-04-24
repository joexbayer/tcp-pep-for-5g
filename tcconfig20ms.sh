sudo tc qdisc del dev enp0s8 root;
sudo tc qdisc add dev enp0s8 root handle 2: netem delay 20ms;
sudo tc qdisc add dev enp0s8 parent 2: handle 3: htb default 10;
sudo tc class add dev enp0s8 parent 3: classid 10 htb rate 10Mbit;
sudo tc qdisc add dev enp0s8 parent 3:10 handle 11: bfifo limit 25000;

# sudo tc class change dev enp0s8 parent 3: classid 10 htb rate 100Mbit;
# sudo tc qdisc change dev enp0s9 parent 3:10 handle 11: bfifo limit 250000;