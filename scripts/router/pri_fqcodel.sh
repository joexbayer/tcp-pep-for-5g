
sudo tc qdisc del dev 10Gf root;
sudo tc qdisc add dev 10Gf root handle 1: htb default 3;

sudo tc class add dev 10Gf parent 1: classid 1:3 htb rate 3Gbit;
sudo tc class add dev 10Gf parent 1: classid 1:4 htb rate 3Gbit;

sudo tc qdisc add dev 10Gf parent 1:3 fq_codel interval 150;
sudo tc qdisc add dev 10Gf parent 1:4 fq_codel interval 150;

sudo tc filter add dev 10Gf protocol ip parent 1: prio 1 u32 match ip protocol 17 0xff flowid 1:4;
