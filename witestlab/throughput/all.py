import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

def read_data(file_name, config_name):
    scenarios = []
    throughputs = []

    with open(file_name, 'r') as file:
        current_scenario = None
        for line in file:
            if line.strip() in ['BFIFO', 'FQ-CoDel', 'PEP']:
                current_scenario = line.strip()
            else:
                if 'Throughput:' in line:
                    parts = line.split()
                    throughput = float(parts[8])  # Extract the throughput value
                    scenarios.append(current_scenario)
                    throughputs.append(throughput)

    return pd.DataFrame({'Configuration': [config_name] * len(scenarios), 'Scenario': scenarios, 'Throughput': throughputs})

# Mapping of file names to custom configuration names
file_config_map = {
    'sl.txt': 'Short Link',
    'sb.txt': 'Short Blockage',
    'lb.txt': 'Long Blockage',
    'mobb.txt': 'Mobility & Blockages'
}

# Read data from all files and apply custom configuration names
all_data = pd.concat([read_data(file, config) for file, config in file_config_map.items()])

# Print statistical information
print(all_data.groupby(['Configuration', 'Scenario'])['Throughput'].agg(['mean', 'std', 'min', 'max']))

# Set the seaborn style with a white grid
sns.set(style="whitegrid")

# Create a bar plot
plt.figure(figsize=(10, 6))
bar_plot = sns.barplot(x='Scenario', y='Throughput', hue='Configuration', data=all_data, ci=None)

plt.title('Throughput Comparison Across Configurations and Scenarios')
plt.ylabel('Throughput (Mbps)')
plt.xlabel('')

# Add horizontal grid lines only
plt.grid(True, axis='y', color='gray', linestyle='--', linewidth=0.5)

# Save the plot as a PDF file
plt.savefig('throughput_comparison_plot.pdf', bbox_inches='tight')

plt.show()
