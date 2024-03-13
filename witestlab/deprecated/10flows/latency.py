import pandas as pd
import sys

def read_file(file_path):
    """ Reads data from a given file and returns a DataFrame. """
    with open(file_path, 'r') as file:
        num_points = int(file.readline().strip())
        num_points = 1000  # Assuming a fixed number of points for simplicity
        data = {'Point': [], 'Latency': []}
        for _ in range(num_points-2):
            line = file.readline()
            if not line:
                break
            point, latency = line.strip().split(' ')
            data['Point'].append(int(point))
            data['Latency'].append(float(latency)/1000)
    return pd.DataFrame(data)

def compute_statistics(df):
    """ Computes statistics for a given DataFrame. """
    return {
        'Mean': df['Latency'].mean(),
        'Median': df['Latency'].median(),
        'Standard Deviation': df['Latency'].std(),
        'Minimum': df['Latency'].min(),
        'Maximum': df['Latency'].max()
    }

def generate_latex_table(stats1, stats2, file1, file2):
    """ Generates a LaTeX table from the given statistics. """
    # Calculating differences
    differences = {stat: stats1[stat] - stats2[stat] for stat in stats1}

    # Constructing LaTeX table
    latex_table = f"""
\\begin{{table}}[h]
\\centering
\\caption{{Comparison of {file1} and {file2} Statistics}}
\\label{{tab:statistics}}
\\begin{{tabular}}{{lcc}}
\\hline
\\textbf{{Statistic}}                & \\textbf{{{file1}}} & \\textbf{{{file2}}} \\\\ \\hline
Mean                              & {stats1['Mean']:.3f}                  & {stats2['Mean']:.3f}                  \\\\
Median                            & {stats1['Median']:.3f}                  & {stats2['Median']:.3f}                  \\\\
Standard Deviation                & {stats1['Standard Deviation']:.4f}                   & {stats2['Standard Deviation']:.4f}                   \\\\
Minimum                           & {stats1['Minimum']:.3f}                  & {stats2['Minimum']:.3f}                  \\\\
Maximum                           & {stats1['Maximum']:.3f}                  & {stats2['Maximum']:.3f}                  \\\\ \\hline
\\multicolumn{{3}}{{l}}{{\\textbf{{Differences}}}}                             \\\\ \\hline
Difference in Mean                & \\multicolumn{{2}}{{c}}{{{differences['Mean']:.4f}}}         \\\\
Difference in Median              & \\multicolumn{{2}}{{c}}{{{differences['Median']:.4f}}}         \\\\
Difference in Standard Deviation  & \\multicolumn{{2}}{{c}}{{{differences['Standard Deviation']:.4f}}}         \\\\
Difference in Minimum             & \\multicolumn{{2}}{{c}}{{{differences['Minimum']:.4f}}}         \\\\
Difference in Maximum             & \\multicolumn{{2}}{{c}}{{{differences['Maximum']:.4f}}}         \\\\ \\hline
\\end{{tabular}}
\\end{{table}}
"""
    print(latex_table)

def compare_files(file1, file2):
    """ Compares two files and prints out statistics. """
    df1 = read_file(file1)
    df2 = read_file(file2)

    stats1 = compute_statistics(df1)
    stats2 = compute_statistics(df2)

    # Generate LaTeX table
    generate_latex_table(stats1, stats2, file1, file2)

# Check if two file paths are provided as command-line arguments
if len(sys.argv) != 3:
    print("Usage: python script_name.py <path_to_file1> <path_to_file2>")
    sys.exit(1)

file1 = sys.argv[1]
file2 = sys.argv[2]
compare_files(file1, file2)
