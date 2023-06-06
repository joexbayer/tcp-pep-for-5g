sudo tc qdisc del dev enp36s0 root;
sudo tc qdisc add dev enp36s0 root fq maxrate 100mbit;
# sudo ip link set dev enp36s0 gso_max_size 1500;