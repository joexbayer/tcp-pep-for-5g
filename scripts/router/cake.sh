#!/bin/bash

# End2End CAKE with UDP Prioritization

# Delete any existing qdisc on 10Gf
sudo tc qdisc del dev 10Gf root;

# Add CAKE qdisc with 15mbit bandwidth limit and besteffort queueing
sudo tc qdisc add dev 10Gf root cake bandwidth 15mbit ethernet besteffort;

# Use iptables to mark UDP traffic for high priority
sudo iptables -t mangle -A POSTROUTING -p udp -j DSCP --set-dscp-class CS6;

echo "CAKE setup with UDP prioritization is complete."
