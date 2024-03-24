for EXPID in sl-cubic sb-cubic lb-cubic mobb-cubic
do
    cat $EXPID-ss.txt | sed -e ':a; /<->$/ { N; s/<->\n//; ba; }'  | grep "iperf3" > $EXPID-ss-processed.txt
    cat $EXPID-ss-processed.txt | awk '{print $1}' > ts-$EXPID.txt
    cat $EXPID-ss-processed.txt | grep -oP '\bcwnd:.*?(\s|$)' |  awk -F '[:,]' '{print $2}' | tr -d ' ' > cwnd-$EXPID.txt
    cat $EXPID-ss-processed.txt | grep -oP '\brtt:.*?(\s|$)' |  awk -F '[:,]' '{print $2}' | tr -d ' '  | cut -d '/' -f 1   > srtt-$EXPID.txt
    cat $EXPID-ss-processed.txt | grep -oP '\bfd=.*?(\s|$)' |  awk -F '[=,]' '{print $2}' | tr -d ')' | tr -d ' '   > fd-$EXPID.txt

    paste ts-$EXPID.txt fd-$EXPID.txt cwnd-$EXPID.txt srtt-$EXPID.txt -d ',' > $EXPID-ss.csv

    cat $EXPID-iperf.txt | grep "\[ " | grep "sec" | grep -v "0.00-1" | awk '{print $3","$4","$8","$10}' | tr -d ']' | sed 's/-/,/g' > $EXPID-iperf.csv
done
