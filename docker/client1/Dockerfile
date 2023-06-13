FROM mppcontainer

RUN apt update
RUN apt upgrade -y

# wget
RUN apt install -y wget

# python3
RUN apt install -y python3 python3-pip
RUN pip3 install pandas
RUN pip3 install matplotlib
RUN pip3 install seaborn
RUN pip3 install aiohttp

# entrypoint
COPY docker/client1/config/entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

# copy python tests
COPY GetParallel.py /root/GetParallel.py
COPY GetSequential.py /root/GetSequential.py

ENTRYPOINT ["/entrypoint.sh"]