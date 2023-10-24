$ sudo sysctl -w net.core.rmem_max=<size>;
$ sudo sysctl -w net.core.wmem_max=<size>;
$ sudo sysctl -w net.core.rmem_default=<size>;
$ sudo sysctl -w net.core.wmem_default=<size>;
$ sudo sysctl -w net.ipv4.tcp_rmem='<min size> <size> < max size>';
$ sudo sysctl -w net.ipv4.tcp_wmem='<min size> <size> < max size>';
$ sudo sysctl -w net.ipv4.tcp_mem='<min size> <size> < max size>';

# Look into:
# /proc/sys/net/core/netdev_max_backlog: Maximum number of packets in the input queue.
# /proc/sys/net/core/somaxconn: Maximum length of the backlog queue for listen().
# /proc/sys/net/ipv4/tcp_max_tw_buckets: Maximum number of timewait sockets.
# /proc/sys/net/ipv4/tcp_fin_timeout: How long to maintain a closed TCP connection in the TIME_WAIT state.
