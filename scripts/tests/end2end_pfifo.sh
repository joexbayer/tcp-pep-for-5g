echo "End2End PFIFO Priority Test"
ssh midna "sudo tc class change dev 10Gf parent 1: classid 1:1 htb rate 15mbit" > /dev/null;
echo "Starting Ultra Ping..."

ssh hylia "cd uio-master-joeba; python2 ultra_ping/echo.py --client 10.100.67.7 --n_packets 600" > /dev/null &
sleep 5;
echo "Starting TCP transfer..."

ssh hylia "cd uio-master-joeba; ./client_file -p" > /dev/null &
sleep 13;
echo "Changing to 70mbit..."

ssh midna "sudo tc class change dev 10Gf parent 1: classid 1:1 htb rate 70mbit" > /dev/null;
wait
ssh hylia "cd ~/uio-master-joeba; mv udp_packetn_latency_pairs results/udp_pep_fqcodel_pri_transfer" > /dev/null

echo "Done!"
