import argparse
import time
import aiohttp
import asyncio
import pandas
import matplotlib.pyplot as plt
import seaborn as sns
import hashlib

parser = argparse.ArgumentParser(prog='Get');
parser.add_argument('url')
parser.add_argument('-c', '--count', default=1)
args = parser.parse_args()

dataFrame = pandas.DataFrame(columns=['url', 'byteSize', 'duration'])

async def get(url, session):
    async with session.get(url, timeout=None) as response:
        start = time.time()
        result = await response.read()
        duration = time.time() - start;
        return {'url': url, 'byteSize': len(result), 'duration': duration, 'datarate': ((len(result) / 1000000) / duration), 'sha1sum': hashlib.sha1(result).hexdigest()}

# https://stackoverflow.com/questions/35196974/aiohttp-set-maximum-number-of-requests-per-second
#

async def main():
    connector = aiohttp.TCPConnector(limit=5000)
    timeout = aiohttp.ClientTimeout(total=None,sock_connect=None,sock_read=None)
    async with aiohttp.ClientSession(connector=connector, timeout=timeout) as session:
        tasks = []
        for number in range(0, int(args.count)):
            url = args.url
            tasks.append(asyncio.ensure_future(get(url, session)))

        results = await asyncio.gather(*tasks)
        for result in results:
            print(result['url'] + ': Get ' + str(result['byteSize']) + ' bytes in ' + str(result['duration']) + ' seconds. (' + str(result['datarate']) + ' MB/s)')
            global dataFrame
            dataFrame = pandas.concat([dataFrame, pandas.DataFrame.from_records([result])], ignore_index=True)

asyncio.run(main())

print(dataFrame.to_string())
print("Overall data rate: " + str(dataFrame['datarate'].sum()))

# PLOT
fig, (ax1, ax2) = plt.subplots(2, 1, sharey=False)
fig.suptitle("Parallel GET " + args.url)
sns.barplot(ax=ax1, data=dataFrame, x=dataFrame.index, y="duration")
ax1.set_title("duration")
sns.barplot(ax=ax2, data=dataFrame, x=dataFrame.index, y="datarate")
ax2.set_title("datarate")

plt.show()

