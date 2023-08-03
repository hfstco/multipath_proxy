import argparse
import urllib.request
import urllib.error
import ssl
import pandas
import matplotlib.pyplot as plt
import seaborn as sns
import hashlib
import time

parser = argparse.ArgumentParser(prog='Get');
parser.add_argument('url')
args = parser.parse_args()

dataFrame = pandas.DataFrame(columns=['url', 'byteSize', 'duration'])

ssssl = ssl.create_default_context()
ssssl.check_hostname = False
ssssl.verify_mode = ssl.CERT_NONE

try:
    start = time.time()
    response = urllib.request.urlopen(args.url, context=ssssl)
    data = response.read()
    duration = time.time() - start

    print('Get ' + args.url + ' in ' + str(duration) + ' (' + str((len(data) / 1000000) / duration) + ' MB/s)')
    dataFrame = pandas.concat([dataFrame, pandas.DataFrame.from_records([{'url': args.url, 'byteSize': len(data), 'duration': duration, 'datarate': (len(data) / 1000000) / duration, 'sha1sum': hashlib.sha1(data).hexdigest()}])], ignore_index=True)

    start = time.time()
    response = urllib.request.urlopen(args.url, context=ssssl)
    data = response.read()
    duration = time.time() - start

    print('Get ' + args.url + ' in ' + str(duration) + ' (' + str((len(data) / 1000000) / duration) + ' MB/s)')
    dataFrame = pandas.concat([dataFrame, pandas.DataFrame.from_records([{'url': args.url, 'byteSize': len(data), 'duration': duration, 'datarate': (len(data) / 1000000) / duration, 'sha1sum': hashlib.sha1(data).hexdigest()}])], ignore_index=True)

    time.sleep(10)

    start = time.time()
    response = urllib.request.urlopen(args.url, context=ssssl)
    data = response.read()
    duration = time.time() - start

    print('Get ' + args.url + ' in ' + str(duration) + ' (' + str((len(data) / 1000000) / duration) + ' MB/s)')
    dataFrame = pandas.concat([dataFrame, pandas.DataFrame.from_records([{'url': args.url, 'byteSize': len(data), 'duration': duration, 'datarate': (len(data) / 1000000) / duration, 'sha1sum': hashlib.sha1(data).hexdigest()}])], ignore_index=True)
except urllib.error.HTTPError as e:
    print("HTTPError: {}".format(e.code))
except urllib.error.URLError as e:
    print('URLError: {}'.format(e.reason))

print(dataFrame.to_string())

fig, (ax1, ax2) = plt.subplots(2, 1, sharey=False)
fig.suptitle("Parallel GET " + args.url)
sns.barplot(ax=ax1, data=dataFrame, x=dataFrame.index, y="duration")
ax1.set_title("duration")
sns.barplot(ax=ax2, data=dataFrame, x=dataFrame.index, y="datarate")
ax2.set_title("datarate")

plt.show()
