import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

def read_data(file_name):
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

    return pd.DataFrame({'Scenario': scenarios, 'Throughput': throughputs})

def print_stats(data):
    stats = data.groupby('Scenario')['Throughput'].agg(['mean', 'std', 'min', 'max'])
    print("Statistical Information:\n", stats)

# Main script
file_name = 'lb.txt'  # Replace with your file path
data = read_data(file_name)

print_stats(data)  # Print statistical information

# Set the seaborn style with a white grid
sns.set(style="whitegrid")

# Create a bar plot for mean values with overlay of individual data points
bar_plot = sns.barplot(x='Scenario', y='Throughput', data=data, ci=None)  # Default color used
sns.stripplot(x='Scenario', y='Throughput', data=data, color='black', jitter=True, alpha=0.6)

# Add horizontal grid lines only
plt.grid(True, axis='y', color='gray', linestyle='--', linewidth=0.5)

plt.title('Throughput of Long blockage scenario')
plt.ylabel('Throughput (Mbps)')
plt.xlabel('')

# Save the plot as a PDF file
plt.savefig('throughput_plot.pdf', bbox_inches='tight')

plt.show()
