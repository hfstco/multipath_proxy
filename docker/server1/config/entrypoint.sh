#!/bin/sh

# configure network
ip route add 192.168.10.0/24 via 192.168.40.1
ip route add 192.168.20.0/24 via 192.168.40.1
ip route add 192.168.21.0/24 via 192.168.40.1
ip route add 192.168.30.0/24 via 192.168.40.1
ip route add 192.168.31.0/24 via 192.168.40.1

# start sshd
/usr/sbin/sshd -f /etc/sshd_config_mpp

# start nginx
nginx -g "daemon off;"
