#!/bin/sh

# configure network
sysctl -w net.ipv4.ip_forward=1

ip route add 172.30.10.0/24 via 172.30.30.2
ip route add 172.30.40.0/24 via 172.30.31.3

#tc qdisc add dev terl0 root handle 1:0 netem delay 15ms rate 2Mbit
#tc qdisc add dev terr0 root handle 1:0 netem delay 15ms rate 2Mbit

#specify link delay and adapt limit
#https://networkengineering.stackexchange.com/questions/52083/ipoib-with-netem-latency-emulation-poor-performance (answer from Dave Taht)
#https://lists.linuxfoundation.org/pipermail/netem/2007-March/001094.html
#https://stackoverflow.com/questions/18792347/what-does-option-limit-in-tc-netem-mean-and-do
#https://serverfault.com/questions/1118827/combine-netem-with-pfifo-fast-at-the-root (without answer yet)
tc qdisc add dev terl0 root handle 1:0 netem delay 15ms limit 1000000 #forward link
tc qdisc add dev terr0 root handle 1:0 netem delay 15ms limit 1000000 #return link

tc qdisc add dev terl0 parent 1:1 handle 10: tbf rate 2Mbit burst 4542 limit 3750 #forward link (one BDP)
tc qdisc add dev terr0 parent 1:1 handle 10: tbf rate 2Mbit burst 4542 limit 3750 #return link (one BDP)

# start iperf
iperf3 -s -D

# start sshd
/usr/sbin/sshd -D -f /etc/sshd_config_mpp