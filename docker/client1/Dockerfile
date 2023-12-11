FROM mppcontainer

RUN DEBIAN_FRONTEND=noninteractive apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get upgrade -y

# wget
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y wget curl

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
COPY TestSlowStart.py /root/TestSlowStart.py

# entrypoint
COPY docker/client1/config/entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

# Install nvm with node and npm
RUN curl -fsSL https://deb.nodesource.com/setup_20.x | bash -
RUN apt install -y nodejs

# google chrome
RUN wget https://dl.google.com/linux/direct/google-chrome-stable_current_amd64.deb
RUN apt install -y ./google-chrome-stable_current_amd64.deb

# browsertime
RUN npm install browsertime -g

ENTRYPOINT ["/entrypoint.sh"]