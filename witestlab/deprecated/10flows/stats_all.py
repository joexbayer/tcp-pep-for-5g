import pandas as pd
import numpy as np

def read_data(file_names):
    """Reads completion times from given files and returns a DataFrame."""
    data = {'File': [], 'Condition': [], 'Completion Time (s)': []}
    
    for file_name in file_names:
        current_condition = None
        with open(file_name, 'r') as file:
            for line in file:
                if 'Without PEP' in line:
                    current_condition = 'Without PEP'
                elif 'With PEP' in line:
                    current_condition = 'With PEP'
                elif 'LOG' in line:
                    time = float(line.split(' - ')[1].strip().replace('s', ''))
                    data['File'].append(file_name)
                    data['Condition'].append(current_condition)
                    data['Completion Time (s)'].append(time)
    return pd.DataFrame(data)

# List of files to read
file_names = ['sl_cubic.txt', 'sb_cubic.txt', 'lb_cubic.txt', 'mobb_cubic.txt']

# Reading data from files
df = read_data(file_names)  

# Grouping by Condition and File, then calculating statistical metrics
stats = df.groupby(['Condition', 'File'])['Completion Time (s)'].agg(['mean', 'std', 'median', 'min', 'max'])

# Calculating overall average statistics for each condition
overall_stats = stats.groupby('Condition').mean()

# LaTeX table format
latex_table = r"""\begin{table}[h!]
\centering
\begin{tabular}{lccccc}
\toprule
Condition & Mean & Std Dev & Median & Min & Max \\
\midrule
"""

# Adding rows for each condition
for condition, values in overall_stats.iterrows():
    latex_table += f"{condition} & {values['mean']:.2f}s & {values['std']:.2f}s & {values['median']:.2f}s & {values['min']:.2f}s & {values['max']:.2f}s\\\\\n"

latex_table += r"""
\bottomrule
\end{tabular}
\caption{Aggregated Flow Completion Time Statistics}
\label{tab:aggregated_fct_statistics}
\end{table}
"""

print(latex_table)
