import re
import matplotlib.pyplot as plt

# Set global font properties for bold and larger text
plt.rcParams.update({'font.size': 12, 'font.weight': 'bold'})

def parse_log_file(file_path):
    """ Parse the log file and return individual throughput values in Mbps. """
    throughputs = []
    with open(file_path, 'r') as file:
        for line in file:
            match = re.search(r'Throughput: (\d+\.?\d*) Mbps', line)
            if match:
                throughput = float(match.group(1))
                throughputs.append(throughput)
    return throughputs

def plot_throughput(file_list, names, y_line1, name1, y_line2, name2):
    """ Plot the average and individual throughput values in a bar graph. """
    plt.figure(figsize=(10, 6))
    max_throughput = 0  # Variable to track the maximum throughput
    for i, file in enumerate(file_list):
        throughputs = parse_log_file(file)
        avg_throughput = sum(throughputs) / len(throughputs) if throughputs else 0
        max_throughput = max(max_throughput, avg_throughput)  # Update max throughput
        plt.bar(i, avg_throughput, color='tab:blue', zorder=3)
        plt.scatter([i] * len(throughputs), throughputs, color='blue', s=10, zorder=3)

    # Adding horizontal striped lines at specified y-values
    plt.axhline(y=y_line1, color='red', linestyle='--', zorder=2)
    plt.text(0, y_line1, name1, verticalalignment='bottom', color='red', fontweight='bold')

    plt.axhline(y=y_line2, color='red', linestyle='--', zorder=2)
    plt.text(0, y_line2, name2, verticalalignment='bottom', color='red', fontweight='bold')

    # Setting labels, title, and x-ticks
    plt.xlabel('', fontweight='bold')
    plt.ylabel('Throughput (Mbps)', fontweight='bold')
    plt.xticks(range(len(names)), names, fontweight='bold')
    plt.title('Throughput Variation for Buffer Sizes', fontweight='bold', fontsize=14)

    # Adding grid lines behind bars
    plt.grid(True, which='both', linestyle='--', linewidth=0.5, zorder=1)
    plt.ylim(0, max_throughput * 1.1)  # 10% more than the max throughput for space

    # Save the plot as a PDF
    plt.savefig('throughput_variation.pdf', bbox_inches='tight')

# List of log files to parse
log_files = ['default.log', 'step2.log', 'step3.log', 'step4.log', 'step5.log', 'step6.log', 'step7.log', 'step8.log', 'step9.log', 'step10.log']  # Replace with actual file names

# Names for each bar
names = ['Default', '15MB', '30MB', '44MB', '64MB', '74MB', '89MB', '104MB', '119MB', '128MB']

# Specify the y-values and names for the horizontal lines
y_line1 = 1074.42  # Example value, replace with actual
y_line2 = 2287.75  # Example value, replace with actual
name1 = "FQ-CoDel"
name2 = "BFIFO"

# Plotting the results
plot_throughput(log_files, names, y_line1, name1, y_line2, name2)
