sudo tc qdisc del dev enp36s0 root;
sudo tc qdisc add dev enp36s0 root handle 2: netem delay 120ms;
sudo tc qdisc add dev enp36s0 parent 2:11 fq_codel;