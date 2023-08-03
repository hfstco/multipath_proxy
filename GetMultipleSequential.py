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
parser.add_argument('-l', '--link', default="default")
parser.add_argument('-p', '--plot')
parser.add_argument('-c', '--count', default=1)
parser.add_argument('-w', '--warmup')
args = parser.parse_args()

dataFrame = pandas.DataFrame(columns=['link', 'url', 'byteSize', 'duration'])

ssssl = ssl.create_default_context()
ssssl.check_hostname = False
ssssl.verify_mode = ssl.CERT_NONE

if args.warmup:
    response = urllib.request.urlopen(args.warmup, context=ssssl)
    data = response.read()

for i in range(0, int(args.count)):
    for url in args.urls:
        try:
            start = time.time()
            response = urllib.request.urlopen(url, context=ssssl)
            data = response.read()
            duration = time.time() - start

            print('Get ' + url + ' in ' + str(duration) + ' (' + str((len(data) / 1000000) / duration) + ' MB/s)')
            dataFrame = pandas.concat([dataFrame, pandas.DataFrame.from_records([{'link': args.link, 'url': url, 'byteSize': len(data) / 1000, 'duration': duration, 'datarate': (len(data) / 1000000) / duration, 'sha1sum': hashlib.sha1(data).hexdigest()}])], ignore_index=True)
        except urllib.error.HTTPError as e:
            print("HTTPError: {}".format(e.code))
        except urllib.error.URLError as e:
            print('URLError: {}'.format(e.reason))

print(dataFrame.to_string())

if args.plot:
    fig, ax1 = plt.subplots()
    sns.lineplot(ax=ax1, data=dataFrame, x="byteSize", y="duration")
    ax1.set_title("byteSize in kBytes")

    plt.ylim(top=4)

    plt.savefig(args.link + ".png")

dataFrame.to_csv(args.link + ".csv", index=False)

