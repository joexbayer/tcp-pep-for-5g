#!/bin/bash

# Define the buffer size variable
#BUFFER_SIZE=67108864
BUFFER_SIZE=2147483647

# Apply the buffer size settings using the variable
sudo sysctl -w net.core.rmem_max=$BUFFER_SIZE;
sudo sysctl -w net.core.wmem_max=$BUFFER_SIZE;
sudo sysctl -w net.core.rmem_default=$BUFFER_SIZE;
sudo sysctl -w net.core.wmem_default=$BUFFER_SIZE;
sudo sysctl -w net.ipv4.tcp_rmem="$BUFFER_SIZE $BUFFER_SIZE $BUFFER_SIZE";
sudo sysctl -w net.ipv4.tcp_wmem="$BUFFER_SIZE $BUFFER_SIZE $BUFFER_SIZE";
sudo sysctl -w net.ipv4.tcp_mem="$BUFFER_SIZE $BUFFER_SIZE $BUFFER_SIZE";

# Look into:
# /proc/sys/net/core/netdev_max_backlog: Maximum number of packets in the input queue.
# /proc/sys/net/core/somaxconn: Maximum length of the backlog queue for listen().
# /proc/sys/net/ipv4/tcp_max_tw_buckets: Maximum number of timewait sockets.
# /proc/sys/net/ipv4/tcp_fin_timeout: How long to maintain a closed TCP connection in the TIME_WAIT state.
