FROM mppcontainer

RUN DEBIAN_FRONTEND=noninteractive apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get upgrade -y

# wget
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y wget

# python3
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y python3 python3-pip
RUN pip3 install pandas
RUN pip3 install matplotlib
RUN pip3 install seaborn
RUN pip3 install aiohttp

# copy python tests
COPY GetParallel.py /root/GetParallel.py
COPY GetSequential.py /root/GetSequential.py
COPY GetMultipleSequential.py /root/GetMultipleSequential.py
COPY GetHeartBeatPacketTest.py /root/GetHeartBeatPacketTest.py

# entrypoint
COPY docker/client1/config/entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]