sudo tc qdisc del dev 10Gf root;
sudo tc qdisc add dev 10Gf root handle 1: htb default 1;

sudo tc class add dev 10Gf parent 1: classid 1:1 htb rate 15mbit ceil 15mbit;

sudo tc qdisc add dev 10Gf parent 1:1 handle 10: pfifo_fast bands 3;

sudo tc filter add dev 10Gf protocol ip parent 1: prio 1 u32 match ip protocol 17 0xff flowid 1:1;

sudo tc filter add dev 10Gf protocol ip parent 1: prio 2 u32 match ip dst 0.0.0.0/0 flowid 1:1;
