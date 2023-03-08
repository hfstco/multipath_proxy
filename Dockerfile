FROM alpine:latest

RUN apk add --update openssh gcc g++ clang gdb cmake make ninja autoconf automake dos2unix tar rsync python3 \
  && rm  -rf /tmp/* /var/cache/apk/*

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
	echo 'Subsystem sftp /usr/lib/ssh/sftp-server'; \
  ) > /etc/sshd_config_mpp

RUN /usr/bin/ssh-keygen -A
RUN ssh-keygen -t rsa -b 4096 -f  /etc/ssh/ssh_host_key

RUN apk add glog-dev libunwind-dev ncurses-dev gtest-dev

EXPOSE 22
CMD ["/usr/sbin/sshd", "-D", "-f", "/etc/sshd_config_mpp"]