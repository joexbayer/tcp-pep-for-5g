import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

# Initialize variables
names = ['sl_cubic.txt', 'sb_cubic.txt', 'lb_cubic.txt', 'mobb_cubic.txt']
titles = ['Static Link', 'Short Blockage', 'Long Blockage', 'Mobility and Blockage']

# Function to process data from file
def process_file(file_name):
    data = {'Condition': [], 'Completion Time (s)': [], 'Flow': []}
    current_condition = None
    with open(file_name, 'r') as file:
        flow_count = 1
        for line in file:
            if 'Without PEP' in line:
                current_condition = 'Without PEP'
                flow_count = 1
            elif 'With PEP' in line:
                current_condition = 'With PEP'
                flow_count = 1
            elif 'LOG' in line:
                time = float(line.split(' - ')[1].strip().replace('s', ''))
                data['Condition'].append(current_condition)
                data['Completion Time (s)'].append(time)
                data['Flow'].append(flow_count)
                flow_count += 1
    return pd.DataFrame(data)

# Determine the maximum completion time across all files
max_completion_time = 0
for file_name in names:
    df = process_file(file_name)
    max_time = df['Completion Time (s)'].max()
    if max_time > max_completion_time:
        max_completion_time = max_time

# Creating a figure with 4 subplots arranged as 2x2
fig, axes = plt.subplots(2, 2, figsize=(10, 10))

# Flatten the axes array for easy indexing
axes = axes.flatten()

# Iterate over each file and create a subplot
for idx, file_name in enumerate(names):
    df = process_file(file_name)
    ax = axes[idx]
    sns.barplot(ax=ax, x='Flow', y='Completion Time (s)', hue='Condition', data=df)
    ax.set_title(titles[idx])
    ax.set_xlabel('Flow')
    ax.set_ylabel('Completion Time (s)')
    ax.set_ylim(0, max_completion_time * 1.1)  # Set the same y-axis limits for all subplots
    ax.legend()

# Adjust layout
plt.tight_layout()

# Save the plot as a PDF file
plt.savefig("combined_plots.pdf", format='pdf')

# Display the plot
plt.show()
