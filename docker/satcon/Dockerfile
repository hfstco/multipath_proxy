FROM mppcontainer

# entrypoint
COPY docker/satcon/config/entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]