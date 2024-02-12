
#ssh midna 'cd uio-master-joeba;EXPID="sl-cubic";rm -f "$EXPID"-ss.txt; while true; do ss --no-header -eipn dst 10.100.67.7 | ts '%.s' | tee -a "$EXPID"-ss.txt; sleep 0.1; done' > /dev/null &
ssh midna 'cd uio-master-joeba/witestlab;bash tputvary.sh mobb' > /dev/null &
ssh hylia 'cd uio-master-joeba;EXPID="sb-cubic";iperf3 -f m -c 10.100.67.7 -C cubic -P 10 -i 0.1 -t 120 | ts '%.s' | tee "$EXPID"-iperf.txt' > /dev/null
#ssh midna 'cd uio-master-joeba; EXPID="sl-cubic"; sudo tcpdump -i 10Ge -s 96 -w "$EXPID"2.pcap & iperf3 -f m -c 10.100.67.7 -C cubic -P 10 -i 0.1 -t 120 | ts "%.s" | tee "$EXPID"-iperf.txt; sudo pkill -SIGINT tcpdump' > /dev/null


wait

tcpdump -i 10Gf -s 96 -w refA.pcap host 10.100.67.6 and host 10.100.67.7
tcpdump -i 10Gf -s 96 -w refB.pcap host 10.100.67.7 and host 10.100.67.6
spp -a 10.100.67.6 -A 10.100.67.7 -f refA.pcap -F refB.pcap -cp


ssh midna 'cd uio-master-joeba;EXPID="sl-cubic";iperf2 -u -f m -c 10.100.67.7 -i 0.1 -t 120 -b 0 | ts '%.s' | tee "$EXPID"-iperf.txt' > /dev/null


ssh hylia 'EXPID="mobb-cubic";rm -f "$EXPID"-ss.txt; while true; do ss --no-header -eipn dst 10.100.67.7 | ts '%.s' | tee -a "$EXPID"-ss.txt; sleep 0.1; done'