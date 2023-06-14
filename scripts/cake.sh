#!/bin/bash

#!/bin/bash

# Clear existing TC configurations (optional)
sudo tc qdisc del dev enp36s0 root;
sudo tc qdisc del dev enp24s0 root;

# Configure Cake on the incoming interface (enp36s0)
sudo sudo tc qdisc add dev enp36s0 root handle 1: cake bandwidth 100Mbit latency 100ms target 5ms;

# Configure Cake on the outgoing interface (enp24s0)
sudo tc qdisc add dev enp24s0 root handle 1: cake bandwidth 10Mbit latency 20ms target 5ms ingress;

# Create parent qdisc for prioritized traffic on the incoming interface (enp36s0)
sudo tc qdisc add dev enp36s0 parent 1:1 handle 10: cake atm overhead 8 mpu 64 mpu 64 ingress;

# Create parent qdisc for prioritized traffic on the outgoing interface (enp24s0)
sudo tc qdisc add dev enp24s0 parent 1:1 handle 10: cake atm overhead 8 mpu 64 mpu 64;

# Apply filters for interactive UDP traffic on the incoming interface (enp36s0)
sudo tc filter add dev enp36s0 parent 1:0 protocol udp prio 1 u32 match ip sport 8888 0xffff flowid 1:1;
sudo tc filter add dev enp36s0 parent 1:0 protocol udp prio 1 u32 match ip dport 8888 0xffff flowid 1:1;

# Apply filters for interactive UDP traffic on the outgoing interface (enp24s0)
sudo tc filter add dev enp24s0 parent 1:0 protocol udp prio 1 u32 match ip sport 8888 0xffff flowid 1:1;
sudo tc filter add dev enp24s0 parent 1:0 protocol udp prio 1 u32 match ip dport 8888 0xffff flowid 1:1;
