FROM mppcontainer

# wget
RUN apt install -y wget

# python3
RUN apt install -y python3

# entrypoint
COPY docker/client1/config/entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]