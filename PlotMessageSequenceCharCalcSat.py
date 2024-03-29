import datetime

import pandas
import pandas as pd
from dateutil import parser as dp
import cv2
import numpy as np

df = pd.DataFrame(columns=['timestamp', 'type', 'direction', 'source', 'destination', 'id', 'size', 'connection'])


def create_packet(timestamp, type, direction, source, destination, id, size, connection):
    entry = dict()
    entry["timestamp"] = timestamp.timestamp() * 1000000
    entry["type"] = type
    entry["direction"] = direction
    entry["source"] = source
    entry["destination"] = destination
    entry["id"] = id
    entry["size"] = size
    entry["connection"] = connection
    return entry


current_id = 3
current_timestamp = datetime.datetime.fromtimestamp(0)

LOCAL = "local"
REMOTE = "remote"
IN = "in"
OUT = "out"
TER_ID = 0
SAT_ID = 1
SOURCE = "192.168.1.1:1111"
DESTINATION = "192.168.1.2:80"

# threeway handshake
# SYN
packet = create_packet(current_timestamp, LOCAL, OUT, SOURCE, DESTINATION, 0, 193, SAT_ID)
df = pd.concat([df, pandas.DataFrame.from_records([packet])], ignore_index=True)

current_timestamp += datetime.timedelta(milliseconds=300)

packet = create_packet(current_timestamp, REMOTE, IN, SOURCE, DESTINATION, 0, 193, SAT_ID)
df = pd.concat([df, pandas.DataFrame.from_records([packet])], ignore_index=True)

# SYN-ACK
packet = create_packet(current_timestamp, REMOTE, IN, SOURCE, DESTINATION, 1, 193, SAT_ID)
df = pd.concat([df, pandas.DataFrame.from_records([packet])], ignore_index=True)

current_timestamp += datetime.timedelta(milliseconds=300)

packet = create_packet(current_timestamp, LOCAL, OUT, SOURCE, DESTINATION, 1, 193, SAT_ID)
df = pd.concat([df, pandas.DataFrame.from_records([packet])], ignore_index=True)

# ACK + data
packet = create_packet(current_timestamp, LOCAL, OUT, SOURCE, DESTINATION, 2, 193, SAT_ID)
df = pd.concat([df, pandas.DataFrame.from_records([packet])], ignore_index=True)

current_timestamp += datetime.timedelta(milliseconds=300)

packet = create_packet(current_timestamp, REMOTE, IN, SOURCE, DESTINATION, 2, 193, SAT_ID)
df = pd.concat([df, pandas.DataFrame.from_records([packet])], ignore_index=True)

# send data
step_size = 100000 / 1500
for i in range(67):
    packet = create_packet(current_timestamp, REMOTE, OUT, SOURCE, DESTINATION, current_id, 1024, SAT_ID)
    df = pd.concat([df, pandas.DataFrame.from_records([packet])], ignore_index=True)
    packet = create_packet(current_timestamp + datetime.timedelta(milliseconds=300), LOCAL, IN, SOURCE, DESTINATION, current_id, 1024, SAT_ID)
    df = pd.concat([df, pandas.DataFrame.from_records([packet])], ignore_index=True)

    current_id += 1
    current_timestamp += datetime.timedelta(microseconds=step_size)

# close packet
# current_timestamp = current_timestamp - datetime.timedelta(microseconds=step_size) + datetime.timedelta(milliseconds=300)
# packet = create_packet(current_timestamp, LOCAL, OUT, SOURCE, DESTINATION, 1, 32, TER_ID)
# df = pd.concat([df, pandas.DataFrame.from_records([packet])], ignore_index=True)
# current_timestamp += datetime.timedelta(milliseconds=15)
#
# packet = create_packet(current_timestamp, REMOTE, IN, SOURCE, DESTINATION, 1, 32, TER_ID)
# df = pd.concat([df, pandas.DataFrame.from_records([packet])], ignore_index=True)
#
# packet = create_packet(current_timestamp, REMOTE, OUT, SOURCE, DESTINATION, current_id, 32, TER_ID)
# df = pd.concat([df, pandas.DataFrame.from_records([packet])], ignore_index=True)
# current_timestamp += datetime.timedelta(milliseconds=15)
#
# packet = create_packet(current_timestamp, LOCAL, IN, SOURCE, DESTINATION, current_id, 32, TER_ID)
# df = pd.concat([df, pandas.DataFrame.from_records([packet])], ignore_index=True)


# draw
max_timestamp = df["timestamp"].max()
min_timestamp = df["timestamp"].min()
diff_timestamp = max_timestamp - min_timestamp

IMG_WIDTH = 1250
IMG_HEIGHT = 2000

us_to_height = IMG_HEIGHT / diff_timestamp

PADDING_LEFT = 250
PADDING_RIGHT = 50
PADDING_TOP = 50
PADDING_BOTTOM = 50

img = np.zeros((IMG_HEIGHT + PADDING_TOP + PADDING_BOTTOM, IMG_WIDTH + PADDING_LEFT + PADDING_RIGHT, 3), np.uint8)
img.fill(255)

# draw timestamps
#step_timestamp = diff_timestamp / 4
step_timestamp = 1200000 / 4
for i in range(5):
    cv2.line(img, (PADDING_LEFT - 50, PADDING_TOP + int(i * step_timestamp * us_to_height)), (PADDING_LEFT, PADDING_TOP + int(i * step_timestamp * us_to_height)), (0, 0, 0), 2)
    cv2.putText(img, str(int(i * step_timestamp / 1000)) + " ms", (50, PADDING_TOP + int(i * step_timestamp * us_to_height) + 8), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 0), 1, 2)

# draw vertical lines
cv2.line(img, (PADDING_LEFT, PADDING_TOP), (PADDING_LEFT, IMG_HEIGHT + PADDING_TOP), (0, 0, 0), 2)
cv2.line(img, (IMG_WIDTH - 1, PADDING_TOP), (IMG_WIDTH - 1, IMG_HEIGHT + PADDING_TOP), (0, 0, 0), 2)

# draw packet lines
for index, row in df[df["type"] == "local"].iterrows():
    if row["connection"] == TER_ID:
        packet_arrival = df.where(df.type == "remote").where(df.direction != row['direction']).where(df.id == row["id"]).where(df.connection == TER_ID).where(df.source == row["source"]).where(df.destination == row["destination"]).dropna().reset_index(drop=True)
    else:
        packet_arrival = df.where(df.type == "remote").where(df.direction != row['direction']).where(df.id == row["id"]).where(df.connection == SAT_ID).where(df.source == row["source"]).where(df.destination == row["destination"]).dropna().reset_index(drop=True)
    if packet_arrival.shape[0] != 1:
        continue
    packet_arrival = packet_arrival.to_dict()
    if row["direction"] == "out":
        if row["connection"] == TER_ID: # ter packet
            color = (0, 255, 0)
        else: # sat packet
            color = (255, 0, 0)
        if row["size"] == 32: # close packet
            color = (0, 0, 255)
        cv2.arrowedLine(img, (PADDING_LEFT, PADDING_TOP + int((row["timestamp"] - min_timestamp) * us_to_height)), (IMG_WIDTH - 2, PADDING_TOP + int((packet_arrival["timestamp"][0] - min_timestamp) * us_to_height)), color, 2, tipLength=0.01)
    else:
        if row["connection"] == SAT_ID:
            color = (255, 0, 0)
        else:
            color = (0, 255, 0)
        if row["size"] == 32:
            color = (0, 0, 255)
        cv2.arrowedLine(img, (IMG_WIDTH - 2, PADDING_TOP + int((packet_arrival["timestamp"][0] - min_timestamp) * us_to_height)), (PADDING_LEFT, PADDING_TOP + int((row["timestamp"] - min_timestamp) * us_to_height)), color, 2, tipLength=0.01)

cv2.imshow("Test", img)
cv2.imwrite("test.jpg", img)
cv2.waitKey(0)
