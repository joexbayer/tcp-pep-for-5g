sudo tc qdisc del dev enp36s0 root;
sudo tc qdisc add dev enp36s0 root handle 2: netem delay 100ms;
#sudo tc qdisc add dev enp36s0 parent 2: handle 3: htb default 10;
#sudo tc class add dev enp36s0 parent 3: classid 10 htb rate 1Mbit;
#sudo tc qdisc add dev enp36s0 parent 3:10 handle 11: bfifo limit 2500;

# BFIFO ROUTER
sudo tc qdisc del dev enp24s0 root;
sudo tc qdisc add dev enp24s0 root handle 2: netem delay 20ms;
sudo tc qdisc add dev enp24s0 parent 2: handle 3: htb default 10;
sudo tc class add dev enp24s0 parent 3: classid 10 htb rate 10Mbit;
sudo tc qdisc add dev enp24s0 parent 3:10 handle 11: bfifo limit 25000;

# FQ_CODEL ROUTER
sudo tc qdisc del dev enp24s0 root;
sudo tc qdisc add dev enp24s0 root handle 1: htb default 11;
sudo tc class add dev enp24s0 parent 1: classid 11 htb rate 20mbit;
sudo tc qdisc add dev enp24s0 parent 1:11 fq_codel;

# PRIO ROUTER
sudo tc qdisc del dev enp24s0 root;
sudo tc qdisc add dev enp24s0 root handle 1: htb default 11;
sudo tc class add dev enp24s0 parent 1: classid 11 htb rate 10mbit;
sudo tc filter add dev enp24s0 parent 1: protocol ip prio 1 u32 match ip src 172.16.10.13 match ip dst 172.16.11.5 flowid 1:11;
sudo tc class add dev enp24s0 parent 1: classid 12 htb rate 10mbit;
sudo tc filter add dev enp24s0 parent 1: protocol ip prio 2 u32 match ip src 172.16.11.254 match ip dst 172.16.11.5 flowid 1:12;
sudo tc filter add dev enp24s0 parent 1: protocol ip prio 2 u32 match ip src 172.16.10.4 match ip dst 172.16.11.5 flowid 1:12
sudo tc qdisc add dev enp24s0 parent 1:11 handle 110: pfifo_fast;
sudo tc qdisc add dev enp24s0 parent 1:12 handle 120: pfifo_fast;

# IP TABLES
iptables -A PREROUTING -t mangle -p tcp --sport telnet -j TOS --set-tos Minimize-Delay