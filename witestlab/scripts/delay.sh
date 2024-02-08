
#ssh midna 'cd uio-master-joeba;EXPID="sl-cubic";rm -f "$EXPID"-ss.txt; while true; do ss --no-header -eipn dst 10.100.67.7 | ts '%.s' | tee -a "$EXPID"-ss.txt; sleep 0.1; done' > /dev/null &
ssh midna 'cd uio-master-joeba/witestlab;bash tputvary.sh sl' > /dev/null &
ssh midna 'cd uio-master-joeba;EXPID="sl-cubic";iperf3 -f m -c 10.100.67.7 -C cubic -P 10 -i 0.1 -t 120 | ts '%.s' | tee "$EXPID"-iperf.txt' > /dev/null
#ssh midna 'cd uio-master-joeba; EXPID="sl-cubic"; sudo tcpdump -i 10Ge -s 96 -w "$EXPID"2.pcap & iperf3 -f m -c 10.100.67.7 -C cubic -P 10 -i 0.1 -t 120 | ts "%.s" | tee "$EXPID"-iperf.txt; sudo pkill -SIGINT tcpdump' > /dev/null


wait