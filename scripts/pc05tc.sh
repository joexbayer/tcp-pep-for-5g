sudo tc qdisc del dev 10Ge root;
sudo tc qdisc add dev 10Ge root handle 2: netem delay 20ms;
sudo tc qdisc add dev 10Ge parent 2:11 fq_codel;