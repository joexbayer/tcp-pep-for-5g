import pandas as pd

def read_data(file_path):
    """Reads data from the given file and returns a DataFrame.

    Args:
        file_path (str): Path to the file containing the data.

    Returns:
        pd.DataFrame: DataFrame with the parsed data.
    """
    with open(file_path, 'r') as file:
        lines = file.readlines()

    # Extracting throughput values
    throughputs = []
    for line in lines:
        throughput, _ = line.strip().split(',')
        throughputs.append(float(throughput))

    return pd.DataFrame({'Throughput': throughputs})

# Use the function to read data
file_path = '../mobb-tput.csv'  # Replace with your actual file path
data = read_data(file_path)

# Now you can compute statistical information like mean and std
mean_throughput = data['Throughput'].mean()
std_throughput = data['Throughput'].std()

print("Mean Throughput:", mean_throughput)
print("Standard Deviation of Throughput:", std_throughput)
