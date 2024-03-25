import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt


csv_files = ['mobb-tput.csv']

sns.set(style="whitegrid")

names = ["Mobility & Blockages"]
index = 0

fig, axs = plt.subplots(1, len(csv_files), figsize=(5 * len(csv_files), 5))  

if len(csv_files) == 1:
    axs = [axs]

global_min_tput = float('inf')
global_max_tput = float('-inf')

for file in csv_files:
    df = pd.read_csv(file, header=None, names=["tput", "tdiff"])
    min_tput = df['tput'].min()
    max_tput = df['tput'].max()
    global_min_tput = min(global_min_tput, min_tput)
    global_max_tput = max(global_max_tput, max_tput)


for idx, file in enumerate(csv_files):
    
    df = pd.read_csv(file, header=None, names=["tput", "tdiff"])
    
    
    df['time'] = df['tdiff'].cumsum()
    
    
    sns.lineplot(data=df, x="time", y="tput", ax=axs[idx])
    
    
    axs[idx].set_title(names[index])
    axs[idx].set_xlabel('Time (s)')

    
    if idx == 0:
        axs[idx].set_ylabel('Capacity (Mbps)')
    else:
        axs[idx].set_ylabel('')
        axs[idx].tick_params(axis='y', which='both', length=0)
        axs[idx].set_yticklabels([])

    
    axs[idx].set_xlim(0, 120)
    axs[idx].set_ylim(global_min_tput-10, global_max_tput+50)

    index += 1

plt.tight_layout()
plt.show()