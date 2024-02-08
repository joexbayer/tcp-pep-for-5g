import matplotlib.pyplot as plt
from datetime import datetime

def plot_time_series(file_path):
    timestamps = []
    rtt_values = []

    with open(file_path, 'r') as file:
        for line in file:
            # Split the line into individual values
            values = line.strip().split()

            # Extract timestamp and RTT value
            timestamp = float(values[1])  # Assuming timestamp is the second value
            rtt = float(values[2])  # Assuming RTT is the third value

            # Convert timestamp to datetime object
            dt_object = datetime.fromtimestamp(timestamp)

            if rtt > 1:
                continue
            # Append timestamp and RTT to lists
            timestamps.append(dt_object)
            rtt_values.append(rtt)

    # Plotting
    plt.figure(figsize=(10, 6))
    plt.plot(timestamps, rtt_values, marker='o', linestyle='-')
    plt.title('RTT Time Series')
    plt.xlabel('Timestamp')
    plt.ylabel('RTT (seconds)')
    plt.xticks(rotation=45)
    plt.grid(True)
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    file_path = "spp.txt"  # Provide the path to your file here
    plot_time_series(file_path)
