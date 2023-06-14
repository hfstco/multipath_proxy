import argparse
import time
import statistics
import urllib.request
import urllib.response
import urllib.error
import ssl
import functools
import pandas
import matplotlib.pyplot as plt
import seaborn as sns
import hashlib

parser = argparse.ArgumentParser(prog='Get');
parser.add_argument('urls', nargs='+')
args = parser.parse_args()

dataFrame = pandas.DataFrame(columns=['url', 'byteSize', 'duration'])

ssssl = ssl.create_default_context()
ssssl.check_hostname = False
ssssl.verify_mode = ssl.CERT_NONE

for url in args.urls:
    try:
        start = time.time()
        response = urllib.request.urlopen(url, context=ssssl)
        data = response.read()
        duration = time.time() - start

        print('Get ' + url + ' in ' + str(duration) + ' (' + str((len(data) / 1000000) / duration) + ' MB/s)')
        dataFrame = pandas.concat([dataFrame, pandas.DataFrame.from_records([{'url': url, 'byteSize': len(data) / 1000000, 'duration': duration, 'datarate': (len(data) / 1000000) / duration, 'sha1sum': hashlib.sha1(data).hexdigest()}])], ignore_index=True)
    except urllib.error.HTTPError as e:
        print("HTTPError: {}".format(e.code))
    except urllib.error.URLError as e:
        print('URLError: {}'.format(e.reason))

print(dataFrame.to_string())

fig, (ax1, ax2) = plt.subplots(2, 1, sharey=False)
sns.barplot(ax=ax1, data=dataFrame, x="byteSize", y="duration")
ax1.set_title("duration in sec | byteSize in MB")
sns.barplot(ax=ax2, data=dataFrame, x="byteSize", y="datarate")
ax2.set_title("datarate in MB/s")

plt.savefig("multiple.png")
