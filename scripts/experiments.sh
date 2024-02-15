UDP_PACKETS=450
TRIES=1
#for (( flows=1; flows <= $TRIES; flows++ )); do
flows=10
ssh midna 'cd uio-master-joeba/witestlab;bash tputvary.sh sl' > /dev/null &
for (( try=1; try <= $TRIES; try++ )); do
    sudo echo "Test $try: $flows flows";
    #ssh router "sudo tc class change dev enp24s0 parent 1: classid 11 htb rate 15mbit" > /dev/null;
    #ssh pc04 "cd uio-master-joeba; python2 ultra_ping/echo.py --client 172.16.11.5 --n_packets $(($UDP_PACKETS+($flows*100)))" > /dev/null & 
    sleep 5;

    for (( flow=1; flow <= $flows; flow++ )); do
        echo "Starting flow ${flow}"
        ssh hylia "cd uio-master-joeba; ./client_file" > /dev/null &
    done

    sleep 13;
    #ssh router "sudo tc class change dev enp24s0 parent 1: classid 11 htb rate 70mbit" > /dev/null;
    wait
    #ssh pc04 "cd ~/uio-master-joeba;mv udp_packetn_latency_pairs results/udp_${flows}_flow/udp_${flows}_flows_${try}_pep" > /dev/null
done
#ssh pc05 "cd ~/uio-master-joeba;mv ./logs/server.log results/udp_${flows}_flow_pep.log" > /dev/null
#done

# BFIFO test
ssh router "sudo tc class change dev enp24s0 parent 1: classid 11 htb rate 15mbit" > /dev/null;