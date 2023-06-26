import re

file_path = "udp_tcp_timestamps.dat"  # Replace with the actual path to your file


tcp_pattern = r'(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d+) .*: Flags \[S\],'
udp_pattern = r'(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d+) .*: UDP,'


pattern = r'(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d+)'

timestamps = []
i = 0

with open(file_path, 'r') as file:
    for line in file:
        timestamp = ""
        match = re.search(pattern, line)
        if match:
            timestamp = match.group(1)

        if "UDP" in line:
            timestamps.append(["UDP", timestamp])
        else:
            #print("TCP " + timestamp)
            timestamps.append(["TCP", timestamp])

for stamp in timestamps:
    if(stamp[0] == "UDP"):
        if(timestamps[i-2][0] == "TCP" and timestamps[i-1][0] == "TCP" and timestamps[i][0] == "UDP" and timestamps[i+1][0] == "TCP" and timestamps[i+2][0] == "TCP"):
            print(f"----------- START {i} ------------")
            print(timestamps[i-2])
            print(timestamps[i-1])
            print(timestamps[i])
            print(timestamps[i+1])
            print(timestamps[i+2])
            print(f"------------ END {i} -----------")
    
    i += 1
        