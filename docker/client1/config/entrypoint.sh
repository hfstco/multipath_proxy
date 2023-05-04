#!/bin/sh

# configure routes
ip route add 172.16.20.0/24 via 172.16.10.2
ip route add 172.16.30.0/24 via 172.16.10.2
ip route add 172.16.21.0/24 via 172.16.10.2
ip route add 172.16.31.0/24 via 172.16.10.2
ip route add 172.16.40.0/24 via 172.16.10.2

# start iperf
# iperf3 -s -D

# run sshd
# /usr/sbin/sshd -D -f /etc/sshd_config_mpp