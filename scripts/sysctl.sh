sudo sysctl -w net.core.rmem_max=67108864;
sudo sysctl -w net.core.wmem_max=67108864;
sudo sysctl -w net.core.rmem_default=67108864;
sudo sysctl -w net.core.wmem_default=67108864;
sudo sysctl -w net.ipv4.tcp_rmem='67108864 67108864 67108864';
sudo sysctl -w net.ipv4.tcp_wmem='67108864 67108864 67108864';
sudo sysctl -w net.ipv4.tcp_mem='67108864 67108864 67108864';
