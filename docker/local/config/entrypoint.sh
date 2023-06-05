#!/bin/sh

# configure network
sysctl -w net.ipv4.ip_forward=1

ip route add 172.30.21.0/24 via 172.30.20.2 #satr
ip route add 172.30.31.0/24 via 172.30.30.2 #terr
ip route add 172.30.40.0/24 via 172.30.30.2 #servers

iptables -t nat -A POSTROUTING -s 172.30.10.0/24 -o terl0 -j MASQUERADE
ip route add default via 172.30.40.254 dev internet0
# tproxy
# echo "8192 2100000 8400000" > /proc/sys/net/ipv4/tcp_mem
echo "8192 2100000 8400000" > /proc/sys/net/ipv4/tcp_rmem
echo "8192 2100000 8400000" > /proc/sys/net/ipv4/tcp_wmem
ip rule add fwmark 1 lookup 100
ip route add local 0.0.0.0/0 dev lo table 100
iptables -t mangle -F
iptables -t mangle -A PREROUTING -i clients0 -p tcp --match multiport --dport 80,8080,443,8443 -j TPROXY --on-port 5000 --tproxy-mark 1

ip route flush cache

ip rule add from all fwmark 177 table 177
iptables -A OUTPUT -t mangle -p tcp --dport 7000 -j MARK --set-mark 177

iptables -A POSTROUTING -t nat -o satr0 -p tcp --dport 7000 -j SNAT --to 172.30.20.1

ip route change default via 172.30.254.254 dev internet0

# start iperf
iperf3 -s -D

# run sshd
/usr/sbin/sshd -D -f /etc/sshd_config_mpp