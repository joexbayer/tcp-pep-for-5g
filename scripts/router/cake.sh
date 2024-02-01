#!/bin/bash

# End2End CAKE with UDP Prioritization


sudo tc qdisc del dev 10Gf root;

sudo tc qdisc add dev 10Gf root cake bandwidth 15mbit ethernet besteffort;

sudo iptables -t mangle -A POSTROUTING -p udp -j DSCP --set-dscp-class EF;

echo "CAKE setup with UDP prioritization is complete."

