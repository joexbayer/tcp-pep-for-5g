# Apply delay on enp36s0
# Apply bandwidth and scheduler on enp24s0
#   if pfifo is used, configure iptables
# sysctl socket buffers

# Delay from router to sender
sudo tc qdisc del dev 10Ge root;
sudo tc qdisc add dev 10Ge root handle 2: netem delay 100ms; #1ms variation
sudo tc qdisc add dev 10Gc parent 2:11 fq_codel;
#sudo tc qdisc add dev enp36s0 parent 2: handle 3: htb default 10;
#sudo tc class add dev enp36s0 parent 3: classid 10 htb rate 1Mbit;
#sudo tc qdisc add dev enp36s0 parent 3:10 handle 11: bfifo limit 2500;

# BFIFO ROUTER
sudo tc qdisc del dev 10Gf root;
sudo tc qdisc add dev 10Gf root handle 2: htb default 10;
sudo tc class add dev 10Gf parent 2: classid 10 htb rate 15mbit;
sudo tc qdisc add dev 10Gf parent 2:10 handle 11: bfifo limit 37500;

# BFIFO STEP
sudo tc qdisc change dev enp24s0 parent 2:10 handle 11: bfifo limit 75000;
sudo tc class change dev enp24s0 parent 2: classid 10 htb rate 30mbit;

# Change ethernet device qlen
sudo ip link set dev enp36s0 txqueuelen 100

# FQ_CODEL / PFIFO ROUTER
sudo tc qdisc del dev 10Gf root;
sudo tc qdisc add dev 10Gf root handle 1: htb default 11;
sudo tc class add dev 10Gf parent 1: classid 11 htb rate 15mbit;
sudo tc qdisc add dev 10Gf parent 1:11 fq_codel interval 150;

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
sudo iptables -A PREROUTING -t mangle -p udp --dport 8888 -j TOS --set-tos Minimize-Delay;
sudo iptables -A PREROUTING -t mangle -p udp --sport 8888 -j TOS --set-tos Minimize-Delay;
sudo iptables -A PREROUTING -t mangle -p tcp --dport 8181 -j TOS --set-tos Maximize-Throughput;
sudo iptables -A PREROUTING -t mangle -p tcp --dport 8183 -j TOS --set-tos Maximize-Throughput;
sudo iptables -A PREROUTING -t mangle -p tcp --sport 8181 -j TOS --set-tos Maximize-Throughput;
sudo iptables -A PREROUTING -t mangle -p tcp --sport 8183 -j TOS --set-tos Maximize-Throughput;


sudo iptables -A OUTPUT -t mangle -p udp --dport 8888 -j TOS --set-tos Minimize-Delay;
sudo iptables -A OUTPUT -t mangle -p udp --sport 8888 -j TOS --set-tos Minimize-Delay;
sudo iptables -A OUTPUT -t mangle -p tcp --dport 8181 -j TOS --set-tos Maximize-Throughput;
sudo iptables -A OUTPUT -t mangle -p tcp --dport 8183 -j TOS --set-tos Maximize-Throughput;
sudo iptables -A OUTPUT -t mangle -p tcp --sport 8181 -j TOS --set-tos Maximize-Throughput;
sudo iptables -A OUTPUT -t mangle -p tcp --sport 8183 -j TOS --set-tos Maximize-Throughput;

# DELETE
sudo iptables -D PREROUTING -t mangle -p udp --dport 8888 -j TOS --set-tos Minimize-Delay;
sudo iptables -D PREROUTING -t mangle -p udp --sport 8888 -j TOS --set-tos Minimize-Delay;
sudo iptables -D PREROUTING -t mangle -p tcp --dport 8181 -j TOS --set-tos Maximize-Throughput;
sudo iptables -D PREROUTING -t mangle -p tcp --dport 8183 -j TOS --set-tos Maximize-Throughput;
sudo iptables -D PREROUTING -t mangle -p tcp --sport 8181 -j TOS --set-tos Maximize-Throughput;
sudo iptables -D PREROUTING -t mangle -p tcp --sport 8183 -j TOS --set-tos Maximize-Throughput;

sudo iptables -D OUTPUT -t mangle -p udp --dport 8888 -j TOS --set-tos Minimize-Delay;
sudo iptables -D OUTPUT -t mangle -p udp --sport 8888 -j TOS --set-tos Minimize-Delay;
sudo iptables -D OUTPUT -t mangle -p tcp --dport 8181 -j TOS --set-tos Maximize-Throughput;
sudo iptables -D OUTPUT -t mangle -p tcp --dport 8183 -j TOS --set-tos Maximize-Throughput;
sudo iptables -D OUTPUT -t mangle -p tcp --sport 8181 -j TOS --set-tos Maximize-Throughput;
sudo iptables -D OUTPUT -t mangle -p tcp --sport 8183 -j TOS --set-tos Maximize-Throughput;


#IPERF
bash tcpss2.sh flow1.dat 3 172.16.11.254 33333 172.16.11.5 5001 &
iperf -c pc05 -B 172.16.11.254:33333 & iperf -c pc05 -B 172.16.11.254:33334 & iperf -c pc05 -B 172.16.11.254:33335;