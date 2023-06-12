sudo tc qdisc del dev enp36s0 root;
sudo tc qdisc add dev enp36s0 root fq maxrate 100mbit;
# sudo ip link set dev enp36s0 gso_max_size 1500;

sudo tc qdisc del dev enp36s0 root;
sudo tc qdisc add dev enp36s0 root handle 2: htb default 10;
sudo tc class add dev enp36s0 parent 2: classid 10 htb rate 100Mbit;
sudo tc qdisc add dev enp36s0 parent 2:10 handle 11: bfifo limit 25000;