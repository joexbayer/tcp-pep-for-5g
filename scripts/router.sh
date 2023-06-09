sudo tc qdisc del dev enp36s0 root;
sudo tc qdisc add dev enp36s0 root handle 2: netem delay 100ms;
#sudo tc qdisc add dev enp36s0 parent 2: handle 3: htb default 10;
#sudo tc class add dev enp36s0 parent 3: classid 10 htb rate 1Mbit;
#sudo tc qdisc add dev enp36s0 parent 3:10 handle 11: bfifo limit 2500;

sudo tc qdisc del dev enp24s0 root;
sudo tc qdisc add dev enp24s0 root handle 2: netem delay 1ms;
sudo tc qdisc add dev enp24s0 parent 2: handle 3: htb default 10;
sudo tc class add dev enp24s0 parent 3: classid 10 htb rate 20Mbit;
#sudo tc qdisc add dev enp24s0 parent 3:10 handle 11: bfifo limit 2500;
sudo tc qdisc add dev enp24s0 parent 3:10 handle 11: fq_codel limit 10000;