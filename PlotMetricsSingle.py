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

# skip 0 lines
# for index, row in df.iterrows():
#     if row[args.type] == 0:
#         df = df.drop(index)
#     else:
#         break

min_timestamp = df['timestamp'].min()
df['timestamp'] = df['timestamp'] - min_timestamp

fig, axs = plt.subplots()

sns.lineplot(x='timestamp',
             y=args.type,
             hue='fd',
             data=df,
             palette='Set1')
axs.set_title(args.type)


plt.show()

