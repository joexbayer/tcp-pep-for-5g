# Final configuration 

## Socket buffers
```bash
BUFFER_SIZE=207374182
sudo sysctl -w net.core.rmem_max=$BUFFER_SIZE;
sudo sysctl -w net.core.wmem_max=$BUFFER_SIZE;
sudo sysctl -w net.core.rmem_default=$BUFFER_SIZE;
sudo sysctl -w net.core.wmem_default=$BUFFER_SIZE;
sudo sysctl -w net.ipv4.tcp_rmem="$BUFFER_SIZE $BUFFER_SIZE $BUFFER_SIZE";
sudo sysctl -w net.ipv4.tcp_wmem="$BUFFER_SIZE $BUFFER_SIZE $BUFFER_SIZE";
sudo sysctl -w net.ipv4.tcp_mem="$BUFFER_SIZE $BUFFER_SIZE $BUFFER_SIZE";
```
## TCP tuning
57374182
28687091
## Defaults
```bash

sudo sysctl -w net.core.rmem_default=212992
sudo sysctl -w net.core.rmem_max=2097152
sudo sysctl -w net.core.wmem_default=212992
sudo sysctl -w net.core.wmem_max=2097152
sudo sysctl -w net.ipv4.tcp_rmem="4096 87380 6291456"
sudo sysctl -w net.ipv4.tcp_wmem="4096 16384 4194304"
sudo sysctl -w net.ipv4.tcp_mem="32768 65536 1536996"

sudo sysctl -w net.core.rmem_default=207374182
sudo sysctl -w net.core.rmem_max=207374182
sudo sysctl -w net.core.wmem_default=207374182
sudo sysctl -w net.core.wmem_max=207374182
sudo sysctl -w net.ipv4.tcp_rmem="4096 87380 207374182"
sudo sysctl -w net.ipv4.tcp_wmem="4096 16384 207374182"
sudo sysctl -w net.ipv4.tcp_mem="32768 65536 207374182"

```

## TC tuning

### Sender
```bash
sudo tc qdisc del dev 10Ge root;
sudo tc qdisc add dev 10Ge root handle 2: netem delay 100ms;
```

### Router
```bash
IP="10.100.67.7"
sudo tc qdisc del dev $(ip route get $IP | grep -oP "(?<=dev )[^ ]+") root; 
sudo tc qdisc replace dev $(ip route get $IP | grep -oP "(?<=dev )[^ ]+") root handle 1: htb default 3;
sudo tc class add dev $(ip route get $IP | grep -oP "(?<=dev )[^ ]+") parent 1: classid 1:3 htb rate 3gbit;
sudo tc qdisc add dev $(ip route get $IP | grep -oP "(?<=dev )[^ ]+") parent 1:3 handle 3: fq_codel interval 150;

sudo tc qdisc add dev $(ip route get $IP | grep -oP "(?<=dev )[^ ]+") parent 1:3 handle 3: bfifo limit 37500000; # BDP  100ms
sudo tc qdisc add dev $(ip route get $IP | grep -oP "(?<=dev )[^ ]+") parent 1:3 handle 3: bfifo limit 37875000; # BDP  100ms
```

#### BFIFO BDP
```bash
```

#### FC-CoDel
```bash

```

### Receiver
```bash
```


Interesting
10ms (11 total)
LOG [2024-03-12 14:52:33] 43169.0mb - 120.000072s - Throughput: 3017.79 Mbps, 377.22 MB/s