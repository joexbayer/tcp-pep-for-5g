import matplotlib.pyplot as plt
import pandas as pd

def read_data(file_name):
    """ Read data from the specified file. """
    with open(file_name, 'r') as file:
        lines = file.readlines()
    return lines

def parse_data(lines):
    """ Parse data into structured form. """
    data_with_pep = []
    data_fq_codel = []
    data_bfifo = []

    category = None

    for line in lines:
        if 'With PEP' in line:
            category = 'with_pep'
            continue
        elif 'End to End (FQ-CoDel)' in line:
            category = 'fq_codel'
            continue
        elif 'End to End (BFIFO)' in line:
            category = 'bfifo'
            continue
        elif 'LOG' in line:
            parts = line.split()
            if len(parts) == 6:
                memory = float(parts[3].replace('mb', ''))
                time = float(parts[5].replace('s', ''))
                throughput = memory / time * 8  # Converted to bits per second

                if category == 'with_pep':
                    data_with_pep.append(throughput)
                elif category == 'fq_codel':
                    data_fq_codel.append(throughput)
                elif category == 'bfifo':
                    data_bfifo.append(throughput)

    return data_with_pep, data_fq_codel, data_bfifo

def calculate_total_throughput(data):
    """ Calculate total throughput. """
    return sum(data)

def calculate_stats(data):
    """ Calculate statistical information. """
    df = pd.DataFrame(data, columns=['Throughput'])
    return df.describe()

def plot_data(data_with_pep, data_fq_codel, data_bfifo):
    """ Plot the throughput data for each category with a shared y-axis. """
    fig, axes = plt.subplots(1, 3, figsize=(15, 5), sharey=True)  # Share y-axis across all subplots

    titles = ['Throughput with PEP', 'Throughput FQ-CoDel', 'Throughput BFIFO']
    data_sets = [data_with_pep, data_fq_codel, data_bfifo]
    
    # Determine the global maximum y-value for consistent scaling
    global_max = max(max(data) for data in data_sets if data)

    for ax, data, title in zip(axes, data_sets, titles):
        if not data:
            print(f"No data available for {title}.")
            continue

        ax.bar(range(len(data)), data, zorder=2)
        ax.set_xticks(range(len(data)))
        ax.grid(axis='y')
        ax.set_title(title)
        ax.set_ylabel('Throughput (Mb/s)')
        ax.set_xlabel('Flows')
        ax.set_ylim(0, global_max + 10)

    plt.tight_layout()
    plt.show()

def calculate_total_throughput(data):
    """ Calculate total throughput in both bytes per second (B/s) and bits per second (b/s). """
    total_throughput_bits = sum(data)  # Assuming original data is in bits per second
    total_throughput_bytes = total_throughput_bits / 8  # Convert to bytes per second
    return total_throughput_bytes, total_throughput_bits


def main():
    file_name = '10flows'  # Replace with your file name
    lines = read_data(file_name)
    data_with_pep, data_fq_codel, data_bfifo = parse_data(lines)

    print (data_with_pep)
    print (data_fq_codel)

    # Plot the data
    plot_data(data_with_pep, data_fq_codel, data_bfifo)

    # Calculating and printing stats
    for category, data in zip(['With PEP', 'FQ-CoDel', 'BFIFO'], [data_with_pep, data_fq_codel, data_bfifo]):
        total_throughput_bytes, total_throughput_bits = calculate_total_throughput(data)
        print(f"\nTotal throughput for {category}: {total_throughput_bytes} B/s, {total_throughput_bits} b/s")

if __name__ == "__main__":
    main()
