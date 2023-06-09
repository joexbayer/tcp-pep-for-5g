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
#sudo tc qdisc del dev enp24s0 root;
#sudo tc qdisc add dev enp24s0 root handle 1: htb default 11;
#sudo tc class add dev enp24s0 parent 1: classid 1:11 htb rate 20mbit;
#sudo tc qdisc add dev enp24s0 parent 1:11 fq_codel;

