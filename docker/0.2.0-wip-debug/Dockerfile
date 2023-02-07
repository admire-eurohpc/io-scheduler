FROM debian:testing-slim

LABEL Description="Debian-based environment suitable to build scord"

ENV DEPS_INSTALL_PATH /usr/local

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
        g++-11 \
        vim \
        gdb \
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
        python3-venv \
        # redis-plus-plus dependencies \
        libhiredis-dev \
        # tests dependencies \
        python3-pip && \
    ### install cmake 3.23.1 ###################################################
    curl -OL https://github.com/Kitware/CMake/releases/download/v3.23.1/cmake-3.23.1-Linux-x86_64.sh && \
    chmod u+x ./cmake-3.23.1-Linux-x86_64.sh && \
    ./cmake-3.23.1-Linux-x86_64.sh --skip-license --prefix=${DEPS_INSTALL_PATH} && \
    rm ./cmake-3.23.1-Linux-x86_64.sh && \
    ###########################################################################
    ###                           DEPENDENCIES
    ########################################################################### \
    cd && \
    mkdir deps && cd deps && \
    git clone https://github.com/jbeder/yaml-cpp --recurse-submodules && \
    git clone https://github.com/json-c/json-c --recurse-submodules && \
    git clone https://github.com/ofiwg/libfabric --recurse-submodules && \
    git clone https://github.com/pmodels/argobots --recurse-submodules && \
    git clone https://github.com/mercury-hpc/mercury --recurse-submodules && \
    git clone https://github.com/mochi-hpc/mochi-margo --recurse-submodules && \
#    cd mochi-margo && git reset --hard v0.9.9 && cd .. && \
    git clone https://github.com/sewenew/redis-plus-plus --recurse-submodules && \
    git clone https://github.com/francielizanon/agios --recurse-submodules && \
    cd agios && git checkout development && cd .. && \
    git clone https://github.com/USCiLab/cereal --recurse-submodules && \
    git clone https://github.com/mochi-hpc/mochi-thallium --recurse-submodules && \
    cd mochi-thallium && \
    export LD_LIBRARY_PATH=${DEPS_INSTALL_PATH}/lib:${DEPS_INSTALL_PATH}/lib64 && \
    export PKG_CONFIG_PATH=${DEPS_INSTALL_PATH}/lib/pkgconfig:${DEPS_INSTALL_PATH}/lib64/pkgconfig && \
    cd && \
    \
    ### yaml-cpp
    cd deps/yaml-cpp && \
    mkdir build && cd build && \
    cmake -DCMAKE_INSTALL_PREFIX=${DEPS_INSTALL_PATH} \
          -DYAML_CPP_BUILD_TESTS=OFF \
          .. && \
    make -j install && \
    cd .. && rm -rf build && cd && \
    \
    ### argobots
    cd deps/argobots && \
    ./autogen.sh && \
    mkdir build && cd build && \
    CFLAGS="-ggdb3 -O0" ../configure --prefix=${DEPS_INSTALL_PATH} && \
    make install -j && \
    cd .. && rm -rf build && cd && \
    cd deps/libfabric && \
    \
    ### libfabric
    ./autogen.sh && \
    mkdir build && cd build && \
    CFLAGS="-ggdb3 -O0" ../configure --prefix=${DEPS_INSTALL_PATH} && \
    make install -j && \
    cd .. && rm -rf build && cd && \
    \
    ### mercury
    cd deps/mercury && \
    mkdir build && cd build && \
    cmake -DMERCURY_USE_SELF_FORWARD:BOOL=ON \
          -DBUILD_TESTING:BOOL=ON \
          -DMERCURY_USE_BOOST_PP:BOOL=ON \
          -DCMAKE_INSTALL_PREFIX=${DEPS_INSTALL_PATH}  \
          -DBUILD_SHARED_LIBS:BOOL=ON \
          -DNA_USE_OFI:BOOL=ON  \
          -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
          -DCMAKE_BUILD_TYPE:STRING=Debug  \
          .. && \
    make install -j && \
    cd .. && rm -rf build && cd && \
    \
    ### json-c
    cd deps/json-c && \
    mkdir build && cd build && \
    cmake -DCMAKE_INSTALL_PREFIX=${DEPS_INSTALL_PATH} \
          .. && \
    make install -j && \
    cd .. && rm -rf build && cd && \
    \
    ### mochi-margo
    cd deps/mochi-margo && \
    ./prepare.sh && \
    mkdir build && cd build && \
    CFLAGS="-ggdb3 -O0" ../configure --prefix=${DEPS_INSTALL_PATH} && \
    make -j install && \
    cd .. && rm -rf build && cd && \
    \
    ### redis-plus-plus
    cd deps/redis-plus-plus && \
    mkdir build && cd build && \
    cmake -DCMAKE_INSTALL_PREFIX=${DEPS_INSTALL_PATH} \
          -DCMAKE_BUILD_TYPE:STRING=Debug \
          .. && \
    make install -j && \
    cd .. && rm -rf build && cd && \
    \
    ### agios
    cd deps/agios && \
    mkdir build && cd build && \
    cmake -DCMAKE_INSTALL_PREFIX=${DEPS_INSTALL_PATH} \
          -DCMAKE_BUILD_TYPE:STRING=Debug \
          .. && \
    make install -j && \
    cd .. && rm -rf build && cd && \
    \
    ### cereal
    cd deps/cereal && \
    mkdir build && cd build && \
    cmake -DCMAKE_BUILD_TYPE:STRING=Debug  \
          -DBUILD_DOC:BOOL=OFF \
          -DBUILD_SANDBOX:BOOL=OFF  \
          -DBUILD_TESTS:BOOL=OFF \
          -DSKIP_PERFORMANCE_COMPARISON:BOOL=ON \
          -DCMAKE_INSTALL_PREFIX=${DEPS_INSTALL_PATH} \
          .. && \
    make -j install && \
    cd .. && rm -rf build && cd && \
    \
    ### mochi-thallium
    cd deps/mochi-thallium && \
    mkdir build && cd build && \
    cmake -DCMAKE_INSTALL_PREFIX=${DEPS_INSTALL_PATH}  \
          -DCMAKE_BUILD_TYPE:STRING=Debug  \
          .. && \
    make -j install && \
    cd .. && rm -rf build && cd && \
    \
    ### python packages for testing scripts\
    pip install lark loguru && \
    \
    ### Cleanup
    # Clean apt cache to reduce image layer size
    rm -rf /var/lib/apt/lists/* && \
    # Clean apt caches of packages
    apt-get clean && apt-get autoclean
