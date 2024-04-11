import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

# Paths to your data files
file_paths = [
    'sl-cubic-ss.csv',  # Replace with your actual file path
    'sb-cubic-ss.csv',  # Replace with your actual file path
    'lb-cubic-ss.csv',  # Replace with your actual file path
    'mobb-cubic-ss.csv'   # Replace with your actual file path
]

names = ["Static Link", "Short Blockages", "Long Blockages", "Mobility & Blockages"]

sns.set(style="whitegrid")  # Set the Seaborn style

# Define the font to use for plots
font = {'family': 'serif',
        'color':  'black',
        'weight': 'bold',
        'size': 16,
        }

# Create a figure with subplots arranged in a single row
fig, axs = plt.subplots(1, len(file_paths), figsize=(5 * len(file_paths), 5), sharey=True)

# If there's only one file, axs will not be an array, so we wrap it in a list
if len(file_paths) == 1:
    axs = [axs]

# Process each file
for idx, file_path in enumerate(file_paths):
    # Read the file into a DataFrame
    df = pd.read_csv(file_path, header=None, names=["Timestamp", "FlowID", "Cwnd", "sRTT"])

    # Convert the timestamp to a datetime format
    df['Timestamp'] = pd.to_datetime(df['Timestamp'], unit='s')

    # Calculate elapsed time in seconds from the first timestamp
    df['Elapsed'] = (df['Timestamp'] - df['Timestamp'].iloc[0]).dt.total_seconds()

    # Resample the data to 1-second intervals and calculate the average for each interval
    df.set_index('Timestamp', inplace=True)
    df_avg = df.resample('1S').mean().reset_index()

    # Calculate the elapsed time for the average data
    df_avg['Elapsed'] = (df_avg['Timestamp'] - df_avg['Timestamp'].iloc[0]).dt.total_seconds()

    # Plotting the data as a line graph on the subplot
    sns.lineplot(data=df_avg, x="Elapsed", y="sRTT", ax=axs[idx], linewidth=2.5)

    # Set the title, xlabel, and ylabel with bold text
    axs[idx].set_title(names[idx], fontdict=font)
    axs[idx].set_xlabel('Time (s)', fontdict=font)
    if idx == 0:  # Only the first subplot gets the ylabel
        axs[idx].set_ylabel('Average sRTT (ms)', fontdict=font)
    
    # Optionally, set the x-axis and y-axis limits to be the same for all plots
    axs[idx].set_xlim(0, 120)
    axs[idx].set_ylim(0, df_avg['sRTT'].max() + 10)  # Set dynamic ylim based on data

    # Set tick labels to bold
    for label in (axs[idx].get_xticklabels() + axs[idx].get_yticklabels()):
        label.set_fontweight('bold')

# Adjust the layout so the plots do not overlap each other
plt.tight_layout()

# Save the plot as a PDF file
pdf_filename = 'combined_plots.pdf'
plt.savefig(pdf_filename, bbox_inches='tight', format='pdf')

# Show the plot
plt.show()
