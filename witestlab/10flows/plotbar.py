import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

# Initialize variables
data = {'Condition': [], 'Completion Time (s)': [], 'Flow': []}
current_condition = None

names = ['sl_cubic.txt', 'sb_cubic.txt', 'lb_cubic.txt', 'mobb_cubic.txt']
titles = ['Static Link', 'Short Blockage', 'Long Blockage', 'Mobility and Blockage']

selector = 2

# Read and process the file
with open(names[selector], 'r') as file:
    flow_count = 1
    for line in file:
        if 'Without PEP' in line:
            current_condition = 'Without PEP'
            flow_count = 1  # Reset flow count for new condition
        elif 'With PEP' in line:
            current_condition = 'With PEP'
            flow_count = 1  # Reset flow count for new condition
        elif 'LOG' in line:
            time = float(line.split(' - ')[1].strip().replace('s', ''))
            data['Condition'].append(current_condition)
            data['Completion Time (s)'].append(time)
            data['Flow'].append(flow_count)
            flow_count += 1

# Creating DataFrame
df = pd.DataFrame(data)

# Split DataFrame by Condition
df_without_pep = df[df['Condition'] == 'Without PEP']
df_with_pep = df[df['Condition'] == 'With PEP']

# Get the default line color from matplotlib
line, = plt.plot(df_without_pep['Completion Time (s)'], linestyle='none') # A dummy plot to get the color
default_line_color = line.get_color()
plt.close()  # Close the dummy plot

# Creating two subplots
fig, axes = plt.subplots(nrows=1, ncols=2, figsize=(12, 4))
max_completion_time = df['Completion Time (s)'].max()

# Setting grid lines
for ax in axes:
    ax.set_axisbelow(True)
    ax.grid(True)
    ax.set_ylim(0, max_completion_time * 1.1)

# Plotting the first bar graph for 'Without PEP' condition
sns.barplot(ax=axes[0], x='Flow', y='Completion Time (s)', data=df_without_pep, color=default_line_color)
axes[0].set_title('Without PEP')
axes[0].set_xlabel('Flow')
axes[0].set_ylabel('Completion Time (s)')

# Plotting the second bar graph for 'With PEP' condition
sns.barplot(ax=axes[1], x='Flow', y='Completion Time (s)', data=df_with_pep, color=default_line_color)
axes[1].set_title('With PEP')
axes[1].set_xlabel('Flow')
axes[1].set_ylabel('Completion Time (s)')

# Adjusting layout
plt.tight_layout()

# Save the plot as a PDF file
pdf_file_path = f"{names[selector].split('.')[0]}_bar.pdf"
plt.savefig(pdf_file_path, format='pdf')

plt.show()
