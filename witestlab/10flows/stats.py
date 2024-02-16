import pandas as pd

def read_data(file_name):
    """Reads completion times from a given file and returns a DataFrame."""
    data = {'Condition': [], 'Completion Time (s)': []}
    current_condition = None

    with open(file_name, 'r') as file:
        for line in file:
            if 'Without PEP' in line:
                current_condition = 'Without PEP'
            elif 'With PEP' in line:
                current_condition = 'With PEP'
            elif 'LOG' in line:
                time = float(line.split(' - ')[1].strip().replace('s', ''))
                data['Condition'].append(current_condition)
                data['Completion Time (s)'].append(time)
    return pd.DataFrame(data)

# Reading data from the file
file_name = 'mobb_cubic.txt'  # Replace with your actual file name
df = read_data(file_name)

# Calculating statistical metrics for each group
stats = df.groupby('Condition')['Completion Time (s)'].agg(['mean', 'std', 'median', 'min', 'max'])

# Convert seconds to milliseconds
stats *= 1

# LaTeX table format
latex_table = r"""\begin{table}[h!]
\centering
\begin{tabularx}{\linewidth}{lCCCCC}
\toprule
Condition & Mean & Std Dev & Median & Min & Max \\
\midrule
"""

# Adding rows for each condition
for condition, values in stats.iterrows():
    latex_table += f"{condition} & {values['mean']:.2f}s & {values['std']:.2f}s & {values['median']:.2f}s & {values['min']:.2f}s & {values['max']:.2f}s\\\\\n"

latex_table += r"""
\bottomrule
\end{tabularx}
\caption{Flow Completion Time Statistics}
\label{tab:fct_statistics}
\end{table}
"""

print(latex_table)
