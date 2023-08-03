import argparse
import time
import aiohttp
import asyncio
import pandas
import matplotlib.pyplot as plt
import seaborn as sns
import hashlib
from time import sleep

parser = argparse.ArgumentParser(prog='Get');
parser.add_argument('url1')
parser.add_argument('url2')
parser.add_argument('-d', '--delay', default=100)
args = parser.parse_args()

dataFrame = pandas.DataFrame(columns=['url', 'byteSize', 'duration'])

async def get(url, session):
    async with session.get(url) as response:
        start = time.time()
        result = await response.read()
        duration = time.time() - start
        return {'url': url, 'byteSize': len(result), 'duration': duration, 'datarate': ((len(result) / 1000000) / duration), 'sha1sum': hashlib.sha1(result).hexdigest()}


async def main():
    async with aiohttp.ClientSession() as session:
        tasks = []
        url = args.url1
        tasks.append(asyncio.ensure_future(get(url, session)))

        sleep(args.delay / 1000)

        url = args.url2
        tasks.append(asyncio.ensure_future(get(url, session)))

        results = await asyncio.gather(*tasks)
        for result in results:
            print(result['url'] + ': Get ' + str(result['byteSize']) + ' bytes in ' + str(result['duration']) + ' seconds. (' + str(result['datarate']) + ' MB/s)')
            global dataFrame
            dataFrame = pandas.concat([dataFrame, pandas.DataFrame.from_records([result])], ignore_index=True)

asyncio.run(main())

print(dataFrame.to_string())
print("Overall data rate: " + str(dataFrame['datarate'].sum()))

