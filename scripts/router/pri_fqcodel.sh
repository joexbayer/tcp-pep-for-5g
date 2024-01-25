
sudo tc qdisc del dev 10Gf root;
sudo tc qdisc add dev 10Gf root handle 1: htb default 11;

sudo tc class add dev 10Gf parent 1: classid 1:11 htb rate 15mbit;
sudo tc class add dev 10Gf parent 1: classid 1:12 htb rate 15mbit;

sudo tc qdisc add dev 10Gf parent 1:11 fq_codel interval 150;
sudo tc qdisc add dev 10Gf parent 1:12 fq_codel interval 150;

sudo tc filter add dev 10Gf protocol ip parent 1: prio 1 u32 match ip protocol 17 0xff flowid 1:12;
