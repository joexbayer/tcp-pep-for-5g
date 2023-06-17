sudo sysctl -w net.core.rmem_max=435544320;
sudo sysctl -w net.core.wmem_max=435544320;
sudo sysctl -w net.core.rmem_default=435544320;
sudo sysctl -w net.core.wmem_default=435544320;
sudo sysctl -w net.ipv4.tcp_rmem='435544320 435544320 435544320';
sudo sysctl -w net.ipv4.tcp_wmem='435544320 435544320 435544320';
sudo sysctl -w net.ipv4.tcp_mem='435544320 435544320 435544320';
