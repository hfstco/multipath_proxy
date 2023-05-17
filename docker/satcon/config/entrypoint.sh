#!/bin/sh

# network configuration
sysctl -w net.ipv4.ip_forward=1

ip route add 172.30.10.0/24 via 172.30.20.1
ip route add 172.30.40.0/24 via 172.30.21.2

tc qdisc add dev satl0 root handle 1:0 netem delay 300ms rate 50Mbit
tc qdisc add dev satr0 root handle 1:0 netem delay 300ms rate 50Mbit

# start iperf
iperf3 -s -D

# start sshd
/usr/sbin/sshd -D -f /etc/sshd_config_mpp