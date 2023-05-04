#!/bin/sh

# configure network
ip route add 172.30.10.0/24 via 172.30.40.1
ip route add 172.30.20.0/24 via 172.30.40.1
ip route add 172.30.21.0/24 via 172.30.40.1
ip route add 172.30.30.0/24 via 172.30.40.1
ip route add 172.30.31.0/24 via 172.30.40.1

# start sshd
/usr/sbin/sshd -f /etc/sshd_config_mpp

# start nginx
nginx -g "daemon off;"
