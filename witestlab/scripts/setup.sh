IP="10.100.67.7"
echo $(ip route get $IP | grep -oP "(?<=dev )[^ ]+"); 
sudo tc qdisc del dev $(ip route get $IP | grep -oP "(?<=dev )[^ ]+") root; 
sudo tc qdisc replace dev $(ip route get $IP | grep -oP "(?<=dev )[^ ]+") root handle 1: htb default 3;
sudo tc class add dev $(ip route get $IP | grep -oP "(?<=dev )[^ ]+") parent 1: classid 1:3 htb rate 10gbit;
sudo tc qdisc add dev $(ip route get $IP | grep -oP "(?<=dev )[^ ]+") parent 1:3 handle 3: fq_codel interval 150;

#sudo tc filter add dev 10Gf protocol ip parent 1: prio 1 u32 match ip protocol 17 0xff flowid 1:1;
#sudo tc qdisc add dev $(ip route get $IP | grep -oP "(?<=dev )[^ ]+") parent 1:3 handle 3: bfifo limit 7500000;

