IP="10.100.67.7"

sudo tc qdisc del dev $(ip route get $IP | grep -oP "(?<=dev )[^ ]+") root; 
sudo tc qdisc replace dev $(ip route get $IP | grep -oP "(?<=dev )[^ ]+") root handle 1: htb default 3;
sudo tc class add dev $(ip route get $IP | grep -oP "(?<=dev )[^ ]+") parent 1: classid 1:3 htb rate 3gbit;
sudo tc qdisc add dev $(ip route get $IP | grep -oP "(?<=dev )[^ ]+") parent 1:3 handle 3: bfifo limit 7500000;
