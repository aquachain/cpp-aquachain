FROM fedora:25

RUN dnf -qy install git cmake
RUN git clone --recursive https://github.com/aquachain/cpp-aquachain --branch build-on-linux --single-branch --depth 2
RUN /cpp-aquachain/scripts/install_deps.sh

RUN cd /tmp && cmake /cpp-aquachain
RUN cd /tmp && make -j8 && make install && ldconfig

ENTRYPOINT ["/usr/local/bin/aqua"]
