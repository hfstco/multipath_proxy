FROM mppcontainer

# docker toolchain
ENV TZ=Europe/London
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN DEBIAN_FRONTEND="noninteractive" apt-get -y install tzdata

RUN apt-get update \
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
      wget \
      vim \
      linux-tools-common \
      linux-tools-generic \
  && apt-get clean

RUN ln -fs /usr/lib/linux-tools/*/perf /usr/bin/perf
RUN echo /proc/sys/kernel/perf_event_paranoid
RUN echo /proc/sys/kernel/kptr_restrict

# mpp
RUN apt install -y libgoogle-glog-dev libunwind-dev ncurses-dev libgtest-dev uuid-dev

# entrypoint
COPY docker/local/config/entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]