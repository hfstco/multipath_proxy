#!/bin/sh

# configure network
ip route add 172.30.10.0/24 via 172.30.40.2
ip route add 172.30.20.0/24 via 172.30.40.2
ip route add 172.30.21.0/24 via 172.30.40.2
ip route add 172.30.30.0/24 via 172.30.40.2
ip route add 172.30.31.0/24 via 172.30.40.2
ip route del default
ip route add default via 172.30.254.1

# start sshd
/usr/sbin/sshd -f /etc/sshd_config_mpp

# start iperf
iperf3 -s -D

# start nginx
nginx -g "daemon off;"
