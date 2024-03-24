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
file_name = 'mobb.txt'  # Replace with your file path
data = read_data(file_name)

print_stats(data)  # Print statistical information

# Create a violin plot
sns.violinplot(x='Scenario', y='Throughput', data=data)
plt.title('Throughput Violin Plot per Scenario')
plt.ylabel('Throughput (Mbps)')
plt.show()
