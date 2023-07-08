import argparse
import matplotlib
import matplotlib.pyplot as plt
import pandas
import seaborn as sns
import pandas as pd
from PIL import Image, ImageDraw
from dateutil import parser as dp
import cv2
import numpy as np

parser = argparse.ArgumentParser()
parser.add_argument('remote_log')
parser.add_argument('local_log')
args = parser.parse_args()

df = pd.DataFrame(columns=['timestamp', 'type', 'direction', 'source', 'destination', 'id', 'size', 'connection'])

# read files
with open(args.remote_log, 'r') as r:
    for line in r:
        parts = line.split()
        if parts[3].startswith("Bond.cpp"):
            entry = dict()
            entry["timestamp"] = int(dp.parse(parts[0][1:] + " " + parts[1]).timestamp() * 1000000)
            entry["type"] = "remote"
            if parts[4].startswith("FlowPacket"):
                # read flowPacket
                entry["direction"] = "out"
                entry["source"] = parts[5].split('=')[1][:-1]
                entry["destination"] = parts[6].split('=')[1][:-1]
                entry["id"] = int(parts[7].split('=')[1][:-1])
                entry["size"] = int(parts[8].split('=')[2][:-1])
                entry["connection"] = int(parts[10].split('=')[1][:-1])
            else:
                # read connection
                entry["direction"] = "in"
                entry["source"] = parts[7].split('=')[1][:-1]
                entry["destination"] = parts[8].split('=')[1][:-1]
                entry["id"] = int(parts[9].split('=')[1][:-1])
                entry["size"] = int(parts[10].split('=')[2][:-1])
                entry["connection"] = int(parts[4].split('=')[1][:-1])

            # write to df
            df = pd.concat([df, pandas.DataFrame.from_records([entry])], ignore_index=True)

with open(args.local_log, 'r') as l:
    for line in l:
        parts = line.split()
        if parts[3].startswith("Bond.cpp"):
            entry = dict()
            entry["timestamp"] = int(dp.parse(parts[0][1:] + " " + parts[1]).timestamp() * 1000000)
            entry["type"] = "local"
            if parts[4].startswith("FlowPacket"):
                # read flowPacket
                entry["direction"] = "out"
                entry["source"] = parts[5].split('=')[1][:-1]
                entry["destination"] = parts[6].split('=')[1][:-1]
                entry["id"] = int(parts[7].split('=')[1][:-1])
                entry["size"] = int(parts[8].split('=')[2][:-1])
                entry["connection"] = int(parts[10].split('=')[1][:-1])
            else:
                # read connection
                entry["direction"] = "in"
                entry["source"] = parts[7].split('=')[1][:-1]
                entry["destination"] = parts[8].split('=')[1][:-1]
                entry["id"] = int(parts[9].split('=')[1][:-1])
                entry["size"] = int(parts[10].split('=')[2][:-1])
                entry["connection"] = int(parts[4].split('=')[1][:-1])

            # write to df
            df = pd.concat([df, pandas.DataFrame.from_records([entry])], ignore_index=True)

# draw
max_timestamp = df["timestamp"].max()
min_timestamp = df["timestamp"].min()
diff_timestamp = max_timestamp - min_timestamp

IMG_WIDTH = 1000
IMG_HEIGHT = 2000

us_to_height = IMG_HEIGHT / diff_timestamp;

img = np.zeros((IMG_HEIGHT, IMG_WIDTH, 3), np.uint8)
img.fill(255)

# draw vertical lines
cv2.line(img, (0, 0), (0, IMG_HEIGHT), (0, 0, 0), 2)
cv2.line(img, (IMG_WIDTH - 1, 0), (IMG_WIDTH - 1, IMG_HEIGHT), (0, 0, 0), 2)
#draw.line((0, 0, 0, IMG_HEIGHT), fill=(0, 0, 0), width=2)
#draw.line((IMG_WIDTH - 2, 0, IMG_WIDTH - 2, IMG_HEIGHT), fill=(0, 0, 0), width=2)

for index, row in df[df["type"] == "local"].iterrows():
    if row["connection"] == 7:
        packet_arrival = df.where(df.type == "remote").where(df.direction != row['direction']).where(df.id == row["id"]).where(df.connection == 8).where(df.source == row["source"]).where(df.destination == row["destination"]).dropna().reset_index(drop=True)
    else:
        packet_arrival = df.where(df.type == "remote").where(df.direction != row['direction']).where(df.id == row["id"]).where(df.connection == 10).where(df.source == row["source"]).where(df.destination == row["destination"]).dropna().reset_index(drop=True)
    assert packet_arrival.shape[0] == 1
    packet_arrival = packet_arrival.to_dict()
    if row["direction"] == "out":
        if row["connection"] == 7:
            color = (255, 0, 0)
        else:
            color = (0, 255, 0)
        if row["size"] == 32:
            color = (0, 0, 255)
        cv2.arrowedLine(img, (0, int((row["timestamp"] - min_timestamp) * us_to_height)), (IMG_WIDTH - 2, int((packet_arrival["timestamp"][0] - min_timestamp) * us_to_height)), color, 2, tipLength=0.01)
    else:
        if row["connection"] == 7:
            color = (255, 0, 0)
        else:
            color = (0, 255, 0)
        if row["size"] == 32:
            color = (0, 0, 255)
        cv2.arrowedLine(img, (IMG_WIDTH - 2, int((packet_arrival["timestamp"][0] - min_timestamp) * us_to_height)), (0, int((row["timestamp"] - min_timestamp) * us_to_height)), color, 2, tipLength=0.01)

cv2.imshow("Test", img)
cv2.imwrite("test.jpg", img)
cv2.waitKey(0)
