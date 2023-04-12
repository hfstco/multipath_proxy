FROM mppcontainer

# entrypoint
COPY docker/tercon/config/entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]