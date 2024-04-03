import matplotlib.pyplot as plt
import os
import re

name_table = {
    './bfifo_10flows.log': 'BFIFO',
    './bfifo_10flows2.log': 'BFIFO2',
    './bfifo_10flows3.log': 'BFIFO3',
    './pep_10_flows.log': 'PEP',
    './fqcodel_10flows.log': 'FQ-CoDel',
    './fqcodel_10flows2.log': 'FQ-CoDel2',
}

# Set global font properties for bold and larger text
plt.rcParams.update({'font.size': 12, 'font.weight': 'bold'})

# Function to parse a single file and return throughput values
def parse_log_file(file_path):
    throughput_values = []
    with open(file_path, 'r') as file:
        for line in file:
            # Using regular expression to extract the throughput value
            match = re.search(r'Throughput: ([\d\.]+) Mbps', line)
            if match:
                throughput = float(match.group(1))
                throughput_values.append(throughput)
    return throughput_values

# Function to plot throughput for each file in a separate subplot and save as PDF
def plot_throughput_for_files(file_paths):
    num_files = len(file_paths)
    max_throughput = 0  # Variable to store the maximum throughput

    # Determine the maximum throughput across all files
    for file_path in file_paths:
        throughput_values = parse_log_file(file_path)
        max_throughput = max(max_throughput, max(throughput_values, default=0))

    # Plot each file's throughput in a separate subplot
    for i, file_path in enumerate(file_paths):
        plt.figure(figsize=(5, 5))
        throughput_values = parse_log_file(file_path)
        plt.bar(range(len(throughput_values)), throughput_values, zorder=3)
        plt.ylim(0, max_throughput * 1.1)  # Slightly higher for visibility
        plt.title(f'{name_table.get(file_path, "Unknown")}', fontweight='bold', fontsize=14)
        plt.xlabel('Flows', fontweight='bold', fontsize=12)
        plt.ylabel('Mbps', fontweight='bold', fontsize=12)
        plt.grid(True, which='both', linestyle='--', linewidth=0.5, zorder=2)
        plt.xticks(range(len(throughput_values)), [str(i+1) for i in range(len(throughput_values))], fontweight='bold', fontsize=10)

        # Save each subplot as a PDF
        pdf_filename = f'{name_table.get(file_path, "subplot")}.pdf'
        plt.savefig(pdf_filename, bbox_inches='tight')
        plt.close()

# Assuming the log files are in the current directory
log_directory = './'
file_paths = [os.path.join(log_directory, file) for file in os.listdir(log_directory) if file.endswith('.log')]

# Plotting the throughput
plot_throughput_for_files(file_paths)
