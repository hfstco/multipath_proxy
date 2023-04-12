#!/bin/sh

# configure network
sysctl -w net.ipv4.ip_forward=1

ip route add 192.168.10.0/24 via 192.168.30.1
ip route add 192.168.40.0/24 via 192.168.31.2

tc qdisc add dev terl0 root handle 1:0 netem delay 15ms rate 2Mbit
tc qdisc add dev terr0 root handle 1:0 netem delay 15ms rate 2Mbit

# start iperf
iperf3 -s -D

# start sshd
/usr/sbin/sshd -D -f /etc/sshd_config_mpp