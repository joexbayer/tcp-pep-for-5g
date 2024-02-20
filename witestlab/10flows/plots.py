import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

def read_data(file_name, file_label):
    """Reads completion times from a given file and returns a DataFrame."""
    data = {'File': [], 'Condition': [], 'Completion Time (s)': []}
    current_condition = None

    with open(file_name, 'r') as file:
        for line in file:
            if 'Without PEP' in line:
                current_condition = 'Without PEP'
            elif 'With PEP' in line:
                current_condition = 'With PEP'
            elif 'LOG' in line:
                time = float(line.split(' - ')[1].strip().replace('s', ''))
                data['File'].append(file_label)
                data['Condition'].append(current_condition)
                data['Completion Time (s)'].append(time)
    return pd.DataFrame(data)

# Reading data from each file
df1 = read_data('sl_cubic.txt', 'Static Link')
df2 = read_data('sb_cubic.txt', 'Short Blockage')
df3 = read_data('lb_cubic.txt', 'Long Blockage')
df4 = read_data('mobb_cubic.txt', 'Mobility and Blockage')

# Combining all DataFrames
df = pd.concat([df1, df2, df3, df4])

# Determine global max and min completion times
global_max_time = df['Completion Time (s)'].max()
global_min_time = df['Completion Time (s)'].min()

# Creating subplots without sharing y-axis
fig, axes = plt.subplots(nrows=2, ncols=2, figsize=(15, 10))
plt.subplots_adjust(hspace=0.5, wspace=0.5)

# Custom palette with light green and light blue
palette = ["#90EE90", "#ADD8E6", "#90EE90", "#ADD8E6"]  # Light Green, Light Blue

# Plotting for each file with grid behind
for i, (ax, file_label) in enumerate(zip(axes.flatten(), ['Static Link', 'Short Blockage', 'Long Blockage', 'Mobility and Blockage'])):
    sns.boxplot(ax=ax, x='Condition', y='Completion Time (s)', data=df[df['File'] == file_label], 
                palette=palette, zorder=2, medianprops={'color': 'black', 'linewidth': 2})
    ax.set_title(file_label)
    ax.grid(True, zorder=1)
    ax.set_axisbelow(True)  # Grid lines behind the plot
    ax.set_xlabel('')
    ax.set_ylabel('Completion Time (s)')
    ax.set_ylim(global_min_time, global_max_time)  # Set the same y-axis scale for all subplots

# Overall title
plt.suptitle('Comparison of Flow Completion Times')

plt.show()
