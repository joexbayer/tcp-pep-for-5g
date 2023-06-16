sudo sysctl -w net.core.rmem_max=335544320;
sudo sysctl -w net.core.wmem_max=335544320;
sudo sysctl -w net.core.rmem_default=335544320;
sudo sysctl -w net.core.wmem_default=335544320;
sudo sysctl -w net.ipv4.tcp_rmem='335544320 335544320 335544320';
sudo sysctl -w net.ipv4.tcp_wmem='335544320 335544320 335544320';
sudo sysctl -w net.ipv4.tcp_mem='335544320 335544320 335544320';
