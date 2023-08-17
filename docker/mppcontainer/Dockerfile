FROM ubuntu:22.04

# update
RUN DEBIAN_FRONTEND=noninteractive apt-get update -y
RUN DEBIAN_FRONTEND=noninteractive apt-get upgrade -y

# general tools
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y vim apt-utils

# ssh
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y openssh-server

RUN echo "root:root" | chpasswd

RUN ( \
	echo 'Port 22'; \
	echo '#AddressFamily any'; \
	echo '#ListenAddress 0.0.0.0'; \
	echo '#ListenAddress ::'; \
	echo 'HostKey /etc/ssh/ssh_host_rsa_key'; \
	echo 'HostKey /etc/ssh/ssh_host_dsa_key'; \
	echo 'HostKey /etc/ssh/ssh_host_ecdsa_key'; \
	echo 'HostKey /etc/ssh/ssh_host_ed25519_key'; \
	echo 'PermitRootLogin yes'; \
	echo '#PasswordAuthentication yes'; \
	echo 'AuthorizedKeysFile .ssh/authorized_keys'; \
	echo '#Subsystem sftp /usr/lib/ssh/sftp-server'; \
    echo 'Subsystem sftp internal-sftp'; \
  ) > /etc/sshd_config_mpp

RUN ssh-keygen -A
RUN ssh-keygen -t rsa -b 4096 -f /etc/ssh/ssh_host_key
RUN mkdir -p /run/sshd

# wireshark
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y wireshark

# network configuration
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y net-tools iputils-ping iptables iproute2

# iperf
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y git build-essential

WORKDIR /root
RUN git clone https://github.com/esnet/iperf.git
WORKDIR /root/iperf
RUN ./configure
RUN make
RUN make install
RUN ldconfig