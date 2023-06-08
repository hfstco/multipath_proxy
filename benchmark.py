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

parser = argparse.ArgumentParser(
    prog='benchmark.py',
    description='foo',
    epilog='bar')

parser.add_argument('urls', nargs='+')           # positional argument
parser.add_argument('-c', '--count', default=10, type=int)
parser.add_argument('-o', '--output')

args = parser.parse_args()

ssssl = ssl.create_default_context()
ssssl.check_hostname = False
ssssl.verify_mode = ssl.CERT_NONE

dataFrame = pandas.DataFrame(columns=['url', 'byteSize', 'duration'])


for url in args.urls:
    for i in range(args.count):
        try:
            start = time.time()
            response = urllib.request.urlopen(url, context=ssssl)
            response.read()
            stop = time.time()
            duration = stop - start
            print(response.status)

            dataFrame = pandas.concat([dataFrame, pandas.DataFrame.from_records([{'url': url, 'byteSize': response.length, 'duration': int(round(duration * 1000))}])])
        except urllib.error.HTTPError as e:
            print("HTTPError: {}".format(e.code))
        except urllib.error.URLError as e:
            print('URLError: {}'.format(e.reason))

sns.lineplot(data=dataFrame, x="byteSize", y="duration", estimator="median", errorbar=("pi", 50))
plt.xscale('log')

pandas.set_option('display.max_rows', None)
pandas.set_option('display.max_columns', None)
pandas.set_option('display.width', None)
pandas.set_option('display.max_colwidth', -1)

print(dataFrame)

if args.output:
    plt.savefig(args.output)
else:
    plt.show()


