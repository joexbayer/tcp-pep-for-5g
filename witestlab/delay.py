import pandas as pd
import matplotlib.pyplot as plt

# Path to your data file
file_path = 'sl-cubic-ss.csv'  # Replace with your actual file path

# Read the file into a DataFrame
df = pd.read_csv(file_path, header=None, names=["Timestamp", "FlowID", "Cwnd", "sRTT"])

# Convert the timestamp to a datetime format
df['Timestamp'] = pd.to_datetime(df['Timestamp'], unit='s')

# Resample the data to 1-second intervals and calculate the average for each interval
df.set_index('Timestamp', inplace=True)
df_avg = df.resample('1S').mean().reset_index()

# Plotting
plt.figure(figsize=(12, 6))

# Create a secondary y-axis for average cwnd
fig, ax1 = plt.subplots()
ax2 = ax1.twinx()

# Plot average sRTT
ax1.plot(df_avg['Timestamp'], df_avg['sRTT'], label='Average sRTT', color='tab:blue')
ax1.set_xlabel('Time (s)')
ax1.set_ylabel('Average sRTT (ms)', color='tab:blue')
ax1.tick_params(axis='y', labelcolor='tab:blue')

# Plot average cwnd
ax2.plot(df_avg['Timestamp'], df_avg['Cwnd'], label='Average Cwnd', color='tab:red')
ax2.set_ylabel('Average Cwnd', color='tab:red')
ax2.tick_params(axis='y', labelcolor='tab:red')

# Title and layout
plt.title('Average sRTT and Cwnd over 120 Seconds')
fig.tight_layout()

plt.show()
