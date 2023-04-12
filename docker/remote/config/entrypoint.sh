#!/bin/sh

# configure network
sysctl -w net.ipv4.ip_forward=1

ip route add 192.168.10.0/24 via 192.168.31.1 #servers
ip route add 192.168.20.0/24 via 192.168.21.1 #satr
ip route add 192.168.30.0/24 via 192.168.31.1 #terr

# start iperf
iperf3 -s -D

# run sshd
/usr/sbin/sshd -D -f /etc/sshd_config_mpp