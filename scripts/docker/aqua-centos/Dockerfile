FROM centos:7

RUN yum -q -y install git
RUN git clone --recursive https://github.com/aquachain/cpp-aquachain --branch build-on-linux --single-branch --depth 8
RUN /cpp-aquachain/scripts/install_cmake.sh --prefix /usr
RUN /cpp-aquachain/scripts/install_deps.sh
RUN yum -y install boost-devel

RUN cd /tmp && cmake /cpp-aquachain
RUN cd /tmp && make -j8 && make install && ldconfig

ENTRYPOINT ["/usr/local/bin/aqua"]
