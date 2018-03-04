FROM debian:jessie

RUN apt-get -q update && \
    apt-get -qy install \
        git \
        curl \
        build-essential \
        libboost-all-dev \
        libgmp-dev \
        libleveldb-dev && \
    git clone --recursive https://github.com/aquachain/cpp-aquachain --branch build-on-linux --single-branch --depth 1 && \
    cpp-aquachain/scripts/install_cmake.sh --prefix /tmp && \
    cd /tmp && PATH=$PATH:/tmp/bin cmake /cpp-aquachain && \
    PATH=$PATH:/tmp/bin cmake --build /tmp -- -j8 && \
    cd /tmp && make install && \
    apt-get purge git build-essential -qy && \
    apt-get autoremove -qy && \
    apt-get clean && \
    rm /cpp-aquachain -rf && \
    rm /tmp/* -rf

ENTRYPOINT ["/usr/local/bin/aqua"]
