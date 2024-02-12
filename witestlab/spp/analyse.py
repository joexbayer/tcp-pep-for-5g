import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from datetime import datetime

def plot_time_series(file_path):
    timestamps = []
    rtt_values = []

    with open(file_path, 'r') as file:
        for line in file:
            values = line.strip().split()
            timestamp = float(values[1])
            rtt = float(values[2]) * 1000
            if rtt > 1:
                continue
            dt_object = datetime.fromtimestamp(timestamp)
            timestamps.append(dt_object)
            rtt_values.append(rtt)

    plt.figure(figsize=(15, 7))  # Adjust the figure size as needed
    plt.plot(timestamps, rtt_values, marker='o', linestyle='-', color='blue', markersize=3, alpha=0.5, linewidth=0.5)
    plt.title('RTT Time Series')
    plt.xlabel('Timestamp')
    plt.ylabel('RTT (milliseconds)')
    plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%H:%M:%S'))  # Format the dates on x-axis
    plt.xticks(rotation=45)
    plt.grid(True)
    plt.tight_layout()
    plt.show()
if __name__ == "__main__":
    
    file_path = "spp2.txt"  
    plot_time_series(file_path)
