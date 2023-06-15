import matplotlib.pyplot as plt
import sys
import datetime
import numpy as np
import pandas as pd
import matplotlib
matplotlib.use('Agg')


def plot_tcp_cwnd(pdf_dir=''):
    for host in [ '.hylia' ]:

        # Load the data from the CSV file
        filename='flowrouter.dat'
        data = pd.read_csv(filename, header=None, sep=',',
                           names=['timestamp',
                                  'source_ip',
                                  'source_port',
                                  'dest_ip',
                                  'dest_port',
                                  'cwnd'])

        # Convert the timestamps to seconds and subtract the first timestamp from all timestamps
        data['timestamp'] = (data['timestamp'] - data['timestamp'].iloc[0])

        # Create a pivot table to separate the data for each flow
        flows = data.pivot_table(values='cwnd',
                                 index='timestamp',
                                 columns=['source_ip',
                                          'source_port',
                                          'dest_ip',
                                          'dest_port'])

        # Replace zero values with NaN
        flows.replace(0, np.nan, inplace=True)

        # Remove rows with NaN values
        # flows.dropna(inplace=True)

        # Plot the data for all flows in one graph
        ax = flows.plot(linewidth=2, fontsize=20)
        ax.set_xlabel('Time (s)', fontsize=20)
        ax.set_ylabel('Congestion Window (pkts.)', fontsize=20)

        ax.grid(True, which='major', lw=0.65, ls='--', dashes=(3, 7), zorder=70)

        # Set the legend outside the graph
        ax.legend(loc='upper center', bbox_to_anchor=(0.5, 1.2), ncol=2)

        # plt.locator_params(axis='x', nbins=11)

        plt.yticks(fontsize=18)
        plt.xticks(fontsize=18)

        plt.grid(True, which='major', lw=0.65, ls='--', dashes=(3, 7), zorder=70)

        ax.set_ylim(bottom=0)

        # Hide the right and top spines
        ax.spines['right'].set_visible(True)
        ax.spines['top'].set_visible(True)

        # Only show ticks on the left and bottom spines
        ax.yaxis.set_ticks_position('left')
        ax.xaxis.set_ticks_position('bottom')

        # ax.get_legend().remove()
        # plt.margins(x=0.02)
        plt.tight_layout(pad=0.4, w_pad=0.5, h_pad=1.0)
        plt.savefig(pdf_dir + '/' + filename + '.pdf',
                format='pdf',
                dpi=1200,
                bbox_inches='tight',
                pad_inches=0.025)


if __name__ == '__main__':
    # Plot cwnd
    plot_tcp_cwnd(pdf_dir='.')
