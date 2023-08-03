import argparse
import matplotlib
import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd

parser = argparse.ArgumentParser(prog='Get')
parser.add_argument('file')
args = parser.parse_args()

df = pd.read_csv(args.file)
min_timestamp = df['timestamp'].min()
df['timestamp'] = df['timestamp'] - min_timestamp

fig, axs = plt.subplots(2, 2)

sns.lineplot(ax=axs[0, 0],
             x='timestamp',
             y='sendDataRate',
             hue='fd',
             data=df,
             palette='Set1')
axs[0, 0].set_title('sendDataRate')

sns.lineplot(ax=axs[1, 0],
             x='timestamp',
             y='sendBufferFillLevel',
             hue='fd',
             data=df,
             palette='Set1')
axs[1, 0].set_title('sendBufferFillLevel')

sns.lineplot(ax=axs[0, 1],
             x='timestamp',
             y='recvDataRate',
             hue='fd',
             data=df,
             palette='Set1')
axs[0, 1].set_title('recvDataRate')

sns.lineplot(ax=axs[1, 1],
             x='timestamp',
             y='recvBufferFillLevel',
             hue='fd',
             data=df,
             palette='Set1')
axs[1, 1].set_title('recvBufferFillLevel')


# sendBufferFillLevel
# sns.lineplot(x='timestamp',
#              y='sendBufferFillLevel',
#              hue='fd',
#              data=df,
#              palette='Set1')
# axs.set_title('sendBufferFillLevel')

# sendDatarate
# sns.lineplot(x='timestamp',
#              y='sendDataRate',
#              hue='fd',
#              data=df,
#              palette='Set1')
# axs.set_title('sendDataRate')

# recvDatarate
# sns.lineplot(x='timestamp',
#              y='recvDataRate',
#              hue='fd',
#              data=df,
#              palette='Set1')
# axs.set_title('recvDataRate')


plt.show()

