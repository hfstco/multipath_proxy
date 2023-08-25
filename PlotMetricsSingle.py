import argparse
import matplotlib
import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd

parser = argparse.ArgumentParser(prog='Get')
parser.add_argument('file')
parser.add_argument('-t', '--type', default='recvDataRate')
args = parser.parse_args()

df = pd.read_csv(args.file)

pal = sns.color_palette('Set1')
colors = pal.as_hex()
colors[0] = colors[2]  # move green to front
colors.pop(2)  # remove red
sns.set_palette(sns.color_palette(colors))

# skip 0 lines
# for index, row in df.iterrows():
#     if row[args.type] == 0:
#         df = df.drop(index)
#     else:
#         break

min_timestamp = df['timestamp'].min()
df['timestamp'] = df['timestamp'] - min_timestamp

# remove fds
df = df.loc[df['fd'] <= 10]
#df['recvDataRate'] = df['recvDataRate'] / 1000000

df['timestamp'] = df['timestamp'] - 180000

fig, axs = plt.subplots()

sns.lineplot(x='timestamp',
             y=args.type,
             hue='fd',
             data=df,
             palette=sns.color_palette(colors))

axs.set_xlim(xmin=0)
plt.xlabel("timestamp in ms")
plt.ylabel("recvDataRate in MB/s")
plt.xlim(xmax=80000)

# get handles
handles, labels = axs.get_legend_handles_labels()
# use them in the legend
labels[0] = "ter"
labels[1] = "sat"
axs.legend(handles[0:10], labels[0:10], title="fd", loc=1)

plt.savefig('plot_metrics_single.pdf', dpi=300)

