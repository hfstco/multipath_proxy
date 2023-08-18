import argparse
import pandas as pd
import re
import seaborn as sns
from matplotlib import pyplot as plt


def convert_string_to_milliseconds(string):
    if string[-3:len(string)] == "ms," or string[-2:len(string)] == "ms":
        return int(string[0:len(string)-3])
    elif string[-2:len(string)] == "s," or string[-1:len(string)] == "s":
        return int(float(string[0:len(string)-2]) * 1000)
    else:
        return 0


parser = argparse.ArgumentParser(prog='Get')
parser.add_argument('ter')
parser.add_argument('sat')
parser.add_argument('bond')
args = parser.parse_args()

ter_df = pd.DataFrame(columns=['website', 'requests', 'TTFB', 'firstPaint', 'FCP', 'DOMContentLoaded', 'LCP', 'CLS', 'TBT', 'CPUBenchmark', 'Load'])
sat_df = pd.DataFrame(columns=['website', 'requests', 'TTFB', 'firstPaint', 'FCP', 'DOMContentLoaded', 'LCP', 'CLS', 'TBT', 'CPUBenchmark', 'Load'])
bond_df = pd.DataFrame(columns=['website', 'requests', 'TTFB', 'firstPaint', 'FCP', 'DOMContentLoaded', 'LCP', 'CLS', 'TBT', 'CPUBenchmark', 'Load'])

with open(args.ter, 'r') as r:
    for line in r:
        parts = line.split()
        entry = dict()
        entry["website"] = parts[3]
        entry["requests"] = int(parts[4])
        entry["TTFB"] = convert_string_to_milliseconds(parts[7])
        entry["firstPaint"] = convert_string_to_milliseconds(parts[9])
        entry["FCP"] = convert_string_to_milliseconds(parts[11])
        entry["DOMContentLoaded"] = convert_string_to_milliseconds(parts[13])
        entry["LCP"] = convert_string_to_milliseconds(parts[15])
        entry["CLS"] = float(parts[17][0:len(parts[17])-1])
        entry["TBT"] = convert_string_to_milliseconds(parts[19])
        entry["CPUBenchmark"] = convert_string_to_milliseconds(parts[21])
        entry["Load"] = convert_string_to_milliseconds(parts[23])

        ter_df = pd.concat([ter_df, pd.DataFrame.from_records([entry])], ignore_index=True)

with open(args.sat, 'r') as r:
    for line in r:
        parts = line.split()
        entry = dict()
        entry["website"] = parts[3]
        entry["requests"] = int(parts[4])
        entry["TTFB"] = convert_string_to_milliseconds(parts[7])
        entry["firstPaint"] = convert_string_to_milliseconds(parts[9])
        entry["FCP"] = convert_string_to_milliseconds(parts[11])
        entry["DOMContentLoaded"] = convert_string_to_milliseconds(parts[13])
        entry["LCP"] = convert_string_to_milliseconds(parts[15])
        entry["CLS"] = float(parts[17][0:len(parts[17])-1])
        entry["TBT"] = convert_string_to_milliseconds(parts[19])
        entry["CPUBenchmark"] = convert_string_to_milliseconds(parts[21])
        entry["Load"] = convert_string_to_milliseconds(parts[23])

        sat_df = pd.concat([sat_df, pd.DataFrame.from_records([entry])], ignore_index=True)

with open(args.bond, 'r') as r:
    for line in r:
        parts = line.split()
        entry = dict()
        entry["website"] = parts[3]
        entry["requests"] = int(parts[4])
        entry["TTFB"] = convert_string_to_milliseconds(parts[7])
        entry["firstPaint"] = convert_string_to_milliseconds(parts[9])
        entry["FCP"] = convert_string_to_milliseconds(parts[11])
        entry["DOMContentLoaded"] = convert_string_to_milliseconds(parts[13])
        entry["LCP"] = convert_string_to_milliseconds(parts[15])
        entry["CLS"] = float(parts[17][0:len(parts[17])-1])
        entry["TBT"] = convert_string_to_milliseconds(parts[19])
        entry["CPUBenchmark"] = convert_string_to_milliseconds(parts[21])
        entry["Load"] = convert_string_to_milliseconds(parts[23])

        bond_df = pd.concat([bond_df, pd.DataFrame.from_records([entry])], ignore_index=True)

ter_df['ds'] = 'ter'
sat_df['ds'] = 'sat'
bond_df['ds'] = 'bond'

df = pd.concat([ter_df, sat_df, bond_df])

colors = ["#55a868", "#4c72b0", "#c44e52"]
customPalette = sns.set_palette(sns.color_palette(colors))

plt.figure(figsize=(10,5))

sns.barplot(x='website', y='Load', hue='ds', data=df, palette=customPalette)
plt.ylabel("Load in ms")
plt.xlabel("")
plt.legend(loc='upper left')

plt.show()