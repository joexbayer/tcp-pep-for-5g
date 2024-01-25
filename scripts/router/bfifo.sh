

sudo tc qdisc del dev 10Gf root;
sudo tc qdisc add dev 10Gf root handle 2: htb default 10;
sudo tc class add dev 10Gf parent 2: classid 10 htb rate 15mbit;
sudo tc qdisc add dev 10Gf parent 2:10 handle 11: bfifo limit 225000; #37500

# Step
sudo tc qdisc change dev 10Gf parent 2:10 handle 11: bfifo limit 175000;
sudo tc class change dev 10Gf parent 2: classid 10 htb rate 70mbit;