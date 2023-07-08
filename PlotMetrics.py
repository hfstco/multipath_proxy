import argparse
import matplotlib
import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd

parser = argparse.ArgumentParser(prog='Get')
parser.add_argument('file')
args = parser.parse_args()

df = pd.read_csv(args.file)

fig, axs = plt.subplots()

# sns.lineplot(ax=axs[0, 0],
#              x='timestamp',
#              y='sendDataRate',
#              hue='fd',
#              data=df,
#              palette='Set1')
# axs[0, 0].set_title('sendDataRate')
#
# sns.lineplot(ax=axs[1, 0],
#              x='timestamp',
#              y='sendBufferFillLevel',
#              hue='fd',
#              data=df,
#              palette='Set1')
# axs[1, 0].set_title('sendBufferFillLevel')

sns.lineplot(x='timestamp',
             y='sendDataRate',
             hue='fd',
             data=df,
             palette='Set1')
axs.set_title('sendDataRate')

# sns.lineplot(ax=axs[1, 1],
#              x='timestamp',
#              y='recvBufferFillLevel',
#              hue='fd',
#              data=df,
#              palette='Set1')
# axs[1, 1].set_title('recvBufferFillLevel')

plt.show()

