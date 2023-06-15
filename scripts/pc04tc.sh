sudo tc qdisc del dev enp36s0 root;
sudo tc qdisc add dev enp36s0 root fq maxrate 50mbit;
# sudo ip link set dev enp36s0 gso_max_size 1500;

sudo tc qdisc del dev enp36s0 root;
sudo tc qdisc add dev enp36s0 root handle 2: htb default 10;
sudo tc class add dev enp36s0 parent 2: classid 10 htb rate 100Mbit;
sudo tc qdisc add dev enp36s0 parent 2:10 handle 11: bfifo limit 25000;

sudo tc qdisc del dev enp36s0 root;
sudo tc qdisc add dev enp36s0 root handle 1: htb default 11;
sudo tc class add dev enp36s0 parent 1: classid 11 htb rate 50mbit;
sudo tc qdisc add dev enp36s0 parent 1:11 fq_codel limit 100;


# IPERF
bash tcpss2.sh flow1.dat 3 172.16.10.4 33333 172.16.11.5 5001 &
iperf -c pc05 -B 172.16.10.4:33333 & iperf -c pc05 -B 172.16.10.4:33334 & iperf -c pc05 -B 172.16.10.4:33335;