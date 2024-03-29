#!/bin/sh

# configure routes
ip route add 172.30.20.0/24 via 172.30.10.3
ip route add 172.30.30.0/24 via 172.30.10.3
ip route add 172.30.21.0/24 via 172.30.10.3
ip route add 172.30.31.0/24 via 172.30.10.3
ip route add 172.30.40.0/24 via 172.30.10.3
ip route del default
ip route add default via 172.30.10.3

# start iperf
iperf3 -s -D

# run sshd
/usr/sbin/sshd -D -f /etc/sshd_config_mpp