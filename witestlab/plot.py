import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

# Names of your CSV files as a list
csv_files = ['sl-tput.csv', 'sb-tput.csv', 'lb-tput.csv', 'mobb-tput.csv']

sns.set(style="whitegrid")

names = ["Static Link", "Short Blockages", "Long Blockages", "Mobility & Blockages"]
index = 0

# Create a figure with subplots arranged in a single row
fig, axs = plt.subplots(1, len(csv_files), figsize=(5 * len(csv_files), 5))  # Adjust the width as needed

# If there's only one file, axs will not be an array, so we wrap it in a list
if len(csv_files) == 1:
    axs = [axs]

# Process each file
for idx, file in enumerate(csv_files):
    # Read the current file into a DataFrame
    df = pd.read_csv(file, header=None, names=["tput", "tdiff"])
    
    # Calculate the cumulative time, assuming tdiff is in seconds
    df['time'] = df['tdiff'].cumsum()
    
    # Plotting the data as a line graph on the subplot
    sns.lineplot(data=df, x="time", y="tput", ax=axs[idx])
    
    # Set the title, xlabel, and ylabel
    axs[idx].set_title(names[index])  # Using the file name without extension as the title
    axs[idx].set_xlabel('Time (s)')
    axs[idx].set_ylabel('Capacity (Mbps)')
    
    # Optionally, set the x-axis and y-axis limits to be the same for all plots
    axs[idx].set_xlim(0, 120)
    axs[idx].set_ylim(0, 4000)

    index += 1

# Adjust the layout so the plots do not overlap each other
plt.tight_layout()

# Show the plot
plt.show()
