#!/bin/sh

# network configuration
sysctl -w net.ipv4.ip_forward=1

ip route add 172.30.10.0/24 via 172.30.20.2
ip route add 172.30.40.0/24 via 172.30.21.3

#sudo tc qdisc add dev enp0s3 root handle 1:0 netem delay 300ms limit 1000000 #forward link
#sudo tc qdisc add dev enp0s8 root handle 1:0 netem delay 300ms limit 1000000 #return link

#sudo tc qdisc add dev enp0s3 parent 1:1 handle 10: tbf rate 50Mbit burst 4542 limit 3750000 #forward link (one BDP)
#sudo tc qdisc add dev enp0s8 parent 1:1 handle 10: tbf rate  5Mbit burst 4542 limit  375000 #return link (one BDP)

#tc qdisc add dev satl0 root handle 1:0 netem delay 300ms rate 50Mbit
#tc qdisc add dev satr0 root handle 1:0 netem delay 300ms rate 50Mbit

#specify link delay and adapt limit
#https://networkengineering.stackexchange.com/questions/52083/ipoib-with-netem-latency-emulation-poor-performance (answer from Dave Taht)
#https://lists.linuxfoundation.org/pipermail/netem/2007-March/001094.html
#https://stackoverflow.com/questions/18792347/what-does-option-limit-in-tc-netem-mean-and-do
#https://serverfault.com/questions/1118827/combine-netem-with-pfifo-fast-at-the-root (without answer yet)
tc qdisc add dev satl0 root handle 1:0 netem delay 300ms limit 1000000 #forward link
tc qdisc add dev satr0 root handle 1:0 netem delay 300ms limit 1000000 #return link

tc qdisc add dev satl0 parent 1:1 handle 10: tbf rate 50Mbit burst 4542 limit 3750000 #forward link (one BDP)
tc qdisc add dev satr0 parent 1:1 handle 10: tbf rate  5Mbit burst 4542 limit  375000 #return link (one BDP)

echo "8192 2100000 8400000" > /proc/sys/net/ipv4/tcp_rmem
echo "8192 2100000 8400000" > /proc/sys/net/ipv4/tcp_wmem

# start iperf
iperf3 -s -D

# start sshd
/usr/sbin/sshd -D -f /etc/sshd_config_mpp