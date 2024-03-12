import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

# Path to the data file
file_path = '10steps'  # Update this to the correct file path

# Read the data from the file
data = pd.read_csv(file_path, sep=' - ', names=['Configuration', 'Throughput'], engine='python')
data['Throughput'] = data['Throughput'].apply(lambda x: float(x.replace(' Mbps', '')))  # Convert throughput to numeric

# Separate the data
line_data = data.iloc[:2]  # 'End to end' and 'Default'
bar_data = data.iloc[2:]  # 'Step 2' to 'Step 10'

# Create a plot
plt.figure(figsize=(10, 6))
ax = plt.gca()

# Plot horizontal lines for the first two entries with labels
for index, row in line_data.iterrows():
    y_value = row['Throughput']
    plt.axhline(y=y_value, color='red', linestyle='--', linewidth=1.5)
    plt.text(0.01, y_value, row['Configuration'], va='center', ha='left', backgroundcolor='white', color='red', transform=ax.get_yaxis_transform())

# Plot the rest as bars
sns.barplot(data=bar_data, x='Configuration', y='Throughput', ax=ax)

plt.ylabel('Throughput (Mbps)')
plt.title('Throughput by Configuration')
plt.xticks(rotation=45)  # Rotate the x labels for better readability

# Add a grid behind the bars
ax.set_axisbelow(True)
plt.grid(True, color='gray', linestyle='--', linewidth=0.5)

plt.show()
