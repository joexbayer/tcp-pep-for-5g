import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

def plot_tcp_cwnd(pdf_dir=''):
    for host in ['.hylia']:
        filename = 'tcp-cwnd.dat'
        data = pd.read_csv(filename, header=None, sep=',',
                           names=['timestamp', 'source_ip', 'source_port',
                                  'dest_ip', 'dest_port', 'cwnd'])

        data['timestamp'] = (data['timestamp'] - data['timestamp'].iloc[0])

        flows = data.pivot_table(values='cwnd',
                                 index='timestamp',
                                 columns=['source_ip', 'source_port', 'dest_ip', 'dest_port'])

        flows.replace(0, np.nan, inplace=True)

        # Set the figure size
        plt.figure(figsize=(15, 8))  # Adjust the width and height as needed

        ax = flows.plot(linewidth=2, fontsize=20)
        # Removed label setting
        ax.grid(True, which='major', lw=0.65, ls='--', dashes=(3, 7), zorder=70)
        plt.yticks(fontsize=18)
        plt.xticks(fontsize=18)
        ax.set_ylim(bottom=0)
        ax.spines['right'].set_visible(True)
        ax.spines['top'].set_visible(True)
        ax.yaxis.set_ticks_position('left')
        ax.xaxis.set_ticks_position('bottom')

        # Improve layout
        plt.tight_layout()

        # Preview the plot
        plt.show()

        # Save the plot to a PDF file
        plt.savefig(pdf_dir + '/' + filename + '.pdf',
                    format='pdf',
                    dpi=1200,
                    bbox_inches='tight',
                    pad_inches=0.025)

if __name__ == '__main__':
    plot_tcp_cwnd(pdf_dir='.')
