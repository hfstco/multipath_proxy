#!/bin/sh

# configure routes
ip route add 192.168.20.0/24 via 192.168.10.2
ip route add 192.168.30.0/24 via 192.168.10.2
ip route add 192.168.21.0/24 via 192.168.10.2
ip route add 192.168.31.0/24 via 192.168.10.2
ip route add 192.168.40.0/24 via 192.168.10.2

# start iperf
iperf3 -s -D

# run sshd
/usr/sbin/sshd -D -f /etc/sshd_config_mpp