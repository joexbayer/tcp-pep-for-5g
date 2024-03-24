# 5G traces Experiement

## based on Witeslab: An Experimental Evaluation of Low Latency Congestion Control over mmWave links
https://witestlab.poly.edu/blog/tcp-mmwave/

## Delay experiment
Receiver
```bash
iperf3 -s -i 0.1 -1  
```

Sender
```bash
EXPID="mobb-cubic"  
rm -f "$EXPID"-ss.txt; while true; do ss --no-header -eipn dst 10.100.67.7 | ts '%.s' | tee -a "$EXPID"-ss.txt; sleep 0.1; done  
```
and 
```bash
EXPID="mobb-cubic";  
iperf3 -f m -c 10.100.67.7 -C cubic -P 10 -i 0.1 -t 120 | ts '%.s' | tee "$EXPID"-iperf.txt  
```

Router
```bash
bash tputvary.sh lb
```

### Configuration
```bash
sudo tc qdisc del dev $(ip route get 10.100.67.7 | grep -oP "(?<=dev )[^ ]+") root  
sudo tc qdisc replace dev $(ip route get 10.100.67.7 | grep -oP "(?<=dev )[^ ]+") root handle 1: htb default 3  
sudo tc class add dev $(ip route get 10.100.67.7 | grep -oP "(?<=dev )[^ ]+") parent 1: classid 1:3 htb rate 3gbit

sudo tc qdisc add dev $(ip route get 10.100.67.7 | grep -oP "(?<=dev )[^ ]+") parent 1:3 handle 3: bfifo limit 7500000  
```


