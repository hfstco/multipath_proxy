FROM mppcontainer

# docker toolchain
ENV TZ=Europe/London
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN DEBIAN_FRONTEND="noninteractive" apt-get -y install tzdata

RUN DEBIAN_FRONTEND=noninteractive apt-get update \
  && apt-get install -y build-essential \
      gcc \
      g++ \
      gdb \
      clang \
      make \
      ninja-build \
      cmake \
      autoconf \
      automake \
      libtool \
      valgrind \
      locales-all \
      dos2unix \
      rsync \
      tar \
      python3 \
      python3-dev \
      python3-pip \
      wget \
      vim \
      linux-tools-common \
      linux-tools-generic \
  && apt-get clean

# perf
RUN ln -fs /usr/lib/linux-tools/*/perf /usr/bin/perf
RUN echo /proc/sys/kernel/perf_event_paranoid
RUN echo /proc/sys/kernel/kptr_restrict

# glog
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y libunwind-dev

WORKDIR /root
RUN git clone https://github.com/google/glog.git
WORKDIR /root/glog
RUN cmake -S . -B build -G "Unix Makefiles"
RUN cmake --build build
RUN cmake --build build --target install

# entrypoint
COPY docker/remote/config/entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

# plot
RUN pip3 install gprof2dot

# gperftools
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y gperf google-perftools libgoogle-perftools-dev

# copy mpp to container
COPY / /opt/mpp/
# compile mpp
WORKDIR /opt/mpp/
RUN cmake .
RUN make

ENTRYPOINT ["/entrypoint.sh"]