import argparse
import matplotlib
import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
import numpy as np

parser = argparse.ArgumentParser(prog='Get')
args = parser.parse_args()


def ter(byteSize):
    return (2 * 15 / 1000) + (byteSize / 250)


def sat(byteSize):
    return (600 / 1000) + (byteSize / 6250)


def implBelowThreshold(byteSize):
        return ter(byteSize)


def implOverThreshold(byteSize):
    return ((15 + 300) / 1000) + (byteSize / 6250)


byteSizes = np.linspace(0, 1000, num=100)
byteSizesBelowThreshold = np.linspace(0, 74.219, num=100)
byteSizesOverThreshold = np.linspace(74.219, 1000, num=100)

plt.plot(byteSizesBelowThreshold, implBelowThreshold(byteSizesBelowThreshold), color='red', label="bond")
plt.plot(byteSizesOverThreshold, implOverThreshold(byteSizesOverThreshold), color='red')
plt.plot(byteSizes, sat(byteSizes), color='blue', label="sat")
plt.plot(byteSizes, ter(byteSizes), color='green', label="ter")
plt.legend(title="link")
plt.ylim(0, 5)
plt.xlim(0, 1000)
plt.xlabel("byteSize")
plt.ylabel("duration")

plt.show()

