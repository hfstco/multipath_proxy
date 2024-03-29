FROM mppcontainer

# nginx
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y nginx openssl ssl-cert
RUN sed -i 's/# listen 443 ssl default_server;/listen 443 ssl default_server;/g' /etc/nginx/sites-available/default
RUN sed -i 's/# listen \[::\]:443 ssl default_server;/listen \[::\]:443 ssl default_server;/g' /etc/nginx/sites-available/default
RUN sed -i 's+# include snippets/snakeoil.conf;+include snippets/snakeoil.conf;+g' /etc/nginx/sites-available/default

RUN openssl req -newkey rsa:4096 -x509 -sha256 -days 3650 -nodes -out /etc/ssl/certs/example.crt -keyout /etc/ssl/private/example.key -subj "/O=tmc-multipath-proxy"

RUN openssl rand -out /var/www/html/file.10kbyte  10000
RUN openssl rand -out /var/www/html/file.50kbyte  50000
RUN openssl rand -out /var/www/html/file.100kbyte  100000
RUN openssl rand -out /var/www/html/file.200kbyte  200000
RUN openssl rand -out /var/www/html/file.300kbyte  300000
RUN openssl rand -out /var/www/html/file.400kbyte  400000
RUN openssl rand -out /var/www/html/file.500kbyte  500000
RUN openssl rand -out /var/www/html/file.600kbyte  600000
RUN openssl rand -out /var/www/html/file.700kbyte  700000
RUN openssl rand -out /var/www/html/file.800kbyte  800000
RUN openssl rand -out /var/www/html/file.900kbyte  900000
RUN openssl rand -out /var/www/html/file.1Mbyte   1000000
RUN openssl rand -out /var/www/html/file.10Mbyte   10000000
RUN openssl rand -out /var/www/html/file.100Mbyte   100000000
RUN openssl rand -out /var/www/html/file.200Mbyte   200000000
RUN openssl rand -out /var/www/html/file.300Mbyte   300000000
RUN openssl rand -out /var/www/html/file.400Mbyte   400000000
RUN openssl rand -out /var/www/html/file.500Mbyte   500000000
RUN openssl rand -out /var/www/html/file.600Mbyte   600000000
RUN openssl rand -out /var/www/html/file.700Mbyte   700000000
RUN openssl rand -out /var/www/html/file.800Mbyte   800000000
RUN openssl rand -out /var/www/html/file.900Mbyte   900000000
RUN openssl rand -out /var/www/html/file.1Gbyte   1000000000

# entrypoint
COPY docker/server1/config/entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]