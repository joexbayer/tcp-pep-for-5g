sudo tc qdisc del dev 10Gc root;
sudo tc qdisc add dev 10Gc root handle 2: netem delay 100ms; #1ms variation
sudo tc qdisc add dev 10Gc parent 2:11 fq_codel interval 150;