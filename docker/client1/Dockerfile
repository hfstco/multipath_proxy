FROM mppcontainer

RUN apt update
RUN apt upgrade -y

# wget
RUN apt install -y wget

# python3
RUN apt install -y python3 python3-pip
RUN pip3 install pandas
RUN pip3 install matplotlib

# entrypoint
COPY docker/client1/config/entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]