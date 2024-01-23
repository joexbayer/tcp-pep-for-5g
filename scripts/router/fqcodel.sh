sudo tc qdisc del dev 10Gf root;
sudo tc qdisc add dev 10Gf root handle 1: htb default 11;
sudo tc class add dev 10Gf parent 1: classid 11 htb rate 15mbit;
sudo tc qdisc add dev 10Gf parent 1:11 fq_codel interval 150;