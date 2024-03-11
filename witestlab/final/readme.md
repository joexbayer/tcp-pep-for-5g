# Final configuration 

## Socket buffers
```bash
BUFFER_SIZE=2073741824
BUFFER_SIZE=204800
sudo sysctl -w net.core.rmem_max=$BUFFER_SIZE;
sudo sysctl -w net.core.wmem_max=$BUFFER_SIZE;
sudo sysctl -w net.core.rmem_default=$BUFFER_SIZE;
sudo sysctl -w net.core.wmem_default=$BUFFER_SIZE;
sudo sysctl -w net.ipv4.tcp_rmem="$BUFFER_SIZE $BUFFER_SIZE $BUFFER_SIZE";
sudo sysctl -w net.ipv4.tcp_wmem="$BUFFER_SIZE $BUFFER_SIZE $BUFFER_SIZE";
sudo sysctl -w net.ipv4.tcp_mem="$BUFFER_SIZE $BUFFER_SIZE $BUFFER_SIZE";
```
## TCP tuning


## Defaults
```bash
# Set the default and maximum receive buffer size for all types of connections
sudo sysctl -w net.core.rmem_default=212992
sudo sysctl -w net.core.rmem_max=212992

# Set the default and maximum send buffer size for all types of connections
sudo sysctl -w net.core.wmem_default=212992
sudo sysctl -w net.core.wmem_max=212992

# Set minimum, default, and maximum buffer size for TCP receive buffers
sudo sysctl -w net.ipv4.tcp_rmem="4096 87380 6291456"

# Set minimum, default, and maximum buffer size for TCP send buffers
sudo sysctl -w net.ipv4.tcp_wmem="4096 16384 4194304"

# Set minimum, pressure, and maximum memory usage for TCP
sudo sysctl -w net.ipv4.tcp_mem="32768 65536 131072"

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
sudo tc qdisc add dev $(ip route get $IP | grep -oP "(?<=dev )[^ ]+") parent 1:3 handle 3: bfifo limit 37500000; # BDP  100ms 
```

#### BFIFO BDP
```bash
```

#### FC-CoDel
```bash
sudo tc qdisc add dev $(ip route get $IP | grep -oP "(?<=dev )[^ ]+") parent 1:3 handle 3: fq_codel interval 150;
```

### Receiver
```bash
```