sudo sysctl -w net.core.rmem_max=212992;
sudo sysctl -w net.core.wmem_max=67108864;
sudo sysctl -w net.core.rmem_default=67108864;
sudo sysctl -w net.core.wmem_default=212992;
sudo sysctl -w net.ipv4.tcp_rmem='189561 252751 67108864';
sudo sysctl -w net.ipv4.tcp_wmem='189561 252751 67108864';
sudo sysctl -w net.ipv4.tcp_mem='189561 252751 67108864';
