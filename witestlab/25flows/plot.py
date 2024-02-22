import matplotlib.pyplot as plt
import pandas as pd

def read_data(file_name):
    """Read data from the specified file."""
    with open(file_name, 'r') as file:
        lines = file.readlines()
    return lines

def parse_data(lines):
    """Parse data into structured form."""
    data_with_pep = []
    data_without_pep = []
    is_with_pep = False

    for line in lines:
        if 'With PEP' in line:
            is_with_pep = True
            continue
        elif 'Without PEP' in line:
            is_with_pep = False
            continue

        parts = line.split()
        if len(parts) == 6 and parts[0] == 'LOG':
            memory = float(parts[3].replace('mb', ''))
            time = float(parts[5].replace('s', ''))
            throughput = memory / time
            if is_with_pep:
                data_with_pep.append(throughput*8)
            else:
                data_without_pep.append(throughput*8)

    return data_with_pep, data_without_pep

def calculate_total_throughput(data):
    """Calculate total throughput."""
    return sum(data)

def calculate_stats(data):
    """Calculate statistical information."""
    df = pd.DataFrame(data, columns=['Throughput'])
    return df.describe()

def main():
    file_name = '10flows'  # Replace with your file name
    lines = read_data(file_name)
    data_with_pep, data_without_pep = parse_data(lines)

    import matplotlib.pyplot as plt

    # Assuming data_with_pep and data_without_pep are already defined
    num_bars_with_pep = len(data_with_pep)
    num_bars_without_pep = len(data_without_pep)

    fig, (ax1, ax2) = plt.subplots(1, 2)  # Create two subplots

    # Plot for 'With PEP'
    ax1.bar(range(num_bars_with_pep), data_with_pep, zorder=2)
    ax1.set_xticks(range(num_bars_with_pep))  # Set x-ticks for each bar
    ax1.grid(axis='y')  # Horizontal grid lines
    ax1.set_title('Throughput with PEP')
    ax1.set_ylabel('Throughput (Mb/s)')
    ax1.set_xlabel('Flows')

    # Plot for 'Without PEP'
    ax2.bar(range(num_bars_without_pep), data_without_pep, zorder=2)
    ax2.set_xticks(range(num_bars_without_pep))  # Set x-ticks for each bar
    ax2.grid(axis='y')  # Horizontal grid lines
    ax2.set_title('Throughput without PEP')
    ax2.set_ylabel('Throughput (Mb/s)')
    ax2.set_xlabel('Flows')

    # Determine the limits for the Y-axis
    y_max = max(max(data_with_pep), max(data_without_pep))
    ax1.set_ylim(0, y_max+10)
    ax2.set_ylim(0, y_max+10)

    plt.tight_layout()
    plt.show()




    total_throughput_with_pep = calculate_total_throughput(data_with_pep)
    total_throughput_without_pep = calculate_total_throughput(data_without_pep)

    print("Total Throughput With PEP: {:.2f} MB/s, {:.2f} Mbit/s".format(total_throughput_with_pep/8, total_throughput_with_pep))
    print("Total Throughput Without PEP: {:.2f} MB/s, {:.2f} Mbit/s".format(total_throughput_without_pep/8, total_throughput_without_pep))

    print("\nStatistics With PEP:")
    print(calculate_stats(data_with_pep))
    print("\nStatistics Without PEP:")
    print(calculate_stats(data_without_pep))

    #plot_data(total_throughput_with_pep, total_throughput_without_pep)

if __name__ == "__main__":
    main()
