FROM debian:bullseye-slim

LABEL Description="Debian-based environment suitable to build scord"

ENV INSTALL_PATH /usr/local


RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        git \
        curl \
        ca-certificates \
        libtool \
        pkg-config \
        make \
        automake \
        gcc \
        g++ \
        procps \
        # AGIOS dependencies
        libconfig-dev \
        # Mercury dependencies
        libltdl-dev \
        lbzip2 \
        # Margo dependencies \
        libjson-c-dev \
        # GekkoFS dependencies
        libboost-program-options-dev \
        uuid-dev  \
        python3 \
        libyaml-dev libcurl4-openssl-dev procps \
        # genopts dependencies
        python3-venv libhiredis-dev && \
    # install cmake 3.14 since it's needed for some dependencies
    curl -OL https://github.com/Kitware/CMake/releases/download/v3.23.1/cmake-3.23.1-Linux-x86_64.sh && \
    chmod u+x ./cmake-3.23.1-Linux-x86_64.sh && \
    ./cmake-3.23.1-Linux-x86_64.sh --skip-license --prefix=${INSTALL_PATH} && \
    # Clean apt cache to reduce image layer size
    rm -rf /var/lib/apt/lists/* && \
    # Clean apt caches of packages
    apt-get clean && apt-get autoclean && \
    rm ./cmake-3.23.1-Linux-x86_64.sh && \
    cmake --version && \
    curl -OL https://github.com/jbeder/yaml-cpp/archive/refs/tags/yaml-cpp-0.6.2.tar.gz && \
    tar -xzf yaml-cpp-0.6.2.tar.gz && \
    cd yaml-cpp-yaml-cpp-0.6.2 && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_PATH} .. && \
    make -j install && \
    cd && \
    rm -rf yaml-cpp-yaml-cpp-0.6.2 && \
    mkdir deps && \
    cd deps && \
    git clone https://github.com/jbeder/yaml-cpp --recurse-submodules && \
    git clone https://github.com/json-c/json-c --recurse-submodules && \
    git clone https://github.com/ofiwg/libfabric --recurse-submodules && \
    git clone https://github.com/pmodels/argobots --recurse-submodules && \
    git clone https://github.com/mercury-hpc/mercury --recurse-submodules && \
    git clone https://github.com/mochi-hpc/mochi-margo --recurse-submodules && \
    git clone https://github.com/sewenew/redis-plus-plus --recurse-submodules && \
    export LD_LIBRARY_PATH=${INSTALL_PATH}/lib:${INSTALL_PATH}/lib64 && \
    export PKG_CONFIG_PATH=${INSTALL_PATH}/lib/pkgconfig:${INSTALL_PATH}/lib64/pkgconfig && \
    cd && \
    cd deps/yaml-cpp && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_PATH} -DYAML_CPP_BUILD_TESTS=OFF .. && \
    make -j install && \
    cd ../../ && \
    cd argobots && \
    ./autogen.sh && \
    ./configure --prefix=${INSTALL_PATH} && \
    make install -j && \
    cd .. && \
    cd libfabric && \
    ./autogen.sh && \
    ./configure --prefix=${INSTALL_PATH} && \
    make install -j && \
    cd .. && \
    cd mercury && \
    mkdir build && \
    cd build && \
    cmake -DMERCURY_USE_SELF_FORWARD:BOOL=ON -DBUILD_TESTING:BOOL=ON -DMERCURY_USE_BOOST_PP:BOOL=ON -DCMAKE_INSTALL_PREFIX=${INSTALL_PATH} -DBUILD_SHARED_LIBS:BOOL=ON -DNA_USE_OFI:BOOL=ON -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DCMAKE_BUILD_TYPE:STRING=Release .. && \
    make install -j && \
    cd ../.. && \
    cd json-c && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_PATH} .. && \
    make install -j && \
    cd ../../ && \
    cd mochi-margo && \
    ./prepare.sh && \
    PKG_CONFIG_PATH=${INSTALL_PATH}/lib/pkgconfig:${INSTALL_PATH}/lib64/pkgconfig ./configure --prefix=${INSTALL_PATH} && \
    make -j install && \
    cd && \
    cd deps/redis-plus-plus && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_PATH} .. && \
    make install -j && \
    cd ../../ && \
    cd && \
    rm -rf deps && \
    # Clean apt cache to reduce image layer size
    rm -rf /var/lib/apt/lists/* && \
    # Clean apt caches of packages
    apt-get clean && apt-get autoclean
