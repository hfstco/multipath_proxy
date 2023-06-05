import argparse
import time
import statistics
import urllib.request
import ssl
import functools

parser = argparse.ArgumentParser(
    prog='benchmark.py',
    description='foo',
    epilog='bar')

parser.add_argument('urls', nargs='+')           # positional argument
parser.add_argument('-c', '--count', default=10)

args = parser.parse_args()

ssssl = ssl.create_default_context()
ssssl.check_hostname=False
ssssl.verify_mode=ssl.CERT_NONE

averages = []
for u in args.urls:
    durations = []
    for i in range(args.count):
        start = time.time()
        urllib.request.urlopen(u, context=ssssl)
        stop = time.time()
        durations.append(stop - start)
    # averages.append(statistics.fmean(durations))
    print(u + " : " +
          "5p: " + str(statistics.quantiles(durations, n=100)[5]) +
          " mean: " + str(statistics.fmean(durations)) +
          " 95p: " + str(statistics.quantiles(durations, n=100)[95]))



