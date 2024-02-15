import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

# Initialize variables
data = {'Condition': [], 'Completion Time (s)': []}
current_condition = None

names = ['sl_cubic.txt', 'sb_cubic.txt', 'lb_cubic.txt', 'mobb_cubic.txt']
titles = ['Static Link', 'Short Blockage', 'Long Blockage', 'Mobility and Blockage']

selector = 1

# Read and process the file
with open(names[selector], 'r') as file:
    for line in file:
        if 'Without PEP' in line:
            current_condition = 'Without PEP'
        elif 'With PEP' in line:
            current_condition = 'With PEP'
        elif 'LOG' in line:
            time = float(line.split(' - ')[1].strip().replace('s', ''))
            print(line.split('-')[1].strip().replace('s', '').replace('.', ','))
            print(current_condition, time)
            data['Condition'].append(current_condition)
            data['Completion Time (s)'].append(time)

# Creating DataFrame
df = pd.DataFrame(data)

plt.figure(figsize=(12, 8))
plt.grid(True)

# Create a boxplot
sns.boxplot(x='Condition', y='Completion Time (s)', data=df, palette=["m", "g"])

# Overlay with a strip plot for individual data points
sns.stripplot(x='Condition', y='Completion Time (s)', data=df, color='black', size=3, jitter=True)

# Add titles and labels
plt.title(f'FCT for 10 Flows ({titles[selector]})')
plt.xlabel('Condition')
plt.ylabel('Completion Time (s)')

plt.show()