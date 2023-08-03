#!/bin/sh

# configure network
sysctl -w net.ipv4.ip_forward=1

ip route add 172.30.10.0/24 via 172.30.31.2 #servers
ip route add 172.30.20.0/24 via 172.30.21.2 #satr
ip route add 172.30.30.0/24 via 172.30.31.2 #terr

echo "8192 2100000 8400000" > /proc/sys/net/ipv4/tcp_rmem
echo "8192 2100000 8400000" > /proc/sys/net/ipv4/tcp_wmem

# start iperf
iperf3 -s -D

# run sshd
/usr/sbin/sshd -D -f /etc/sshd_config_mpp