<div align="center">
<h1> scord </h1>

[![build status)](https://img.shields.io/gitlab/pipeline-status/eu/admire/io-scheduler?gitlab_url=https%3A%2F%2Fstorage.bsc.es%2Fgitlab%2F&logo=gitlab)](https://img.shields.io/gitlab/pipeline-status/eu/admire/io-scheduler?gitlab_url=https%3A%2F%2Fstorage.bsc.es%2Fgitlab%2F&logo=gitlab)
[![coverage report](https://storage.bsc.es/gitlab/eu/admire/io-scheduler/badges/main/coverage.svg)](https://storage.bsc.es/gitlab/eu/admire/io-scheduler/-/commits/main)
[![latest release](https://storage.bsc.es/gitlab/eu/admire/io-scheduler/-/badges/release.svg)](https://storage.bsc.es/gitlab/eu/admire/io-scheduler/-/releases)
[![GitLab (self-managed)](https://img.shields.io/gitlab/license/eu/admire/io-scheduler?gitlab_url=https%3A%2F%2Fstorage.bsc.es%2Fgitlab)](https://img.shields.io/gitlab/license/eu/admire/io-scheduler?gitlab_url=https%3A%2F%2Fstorage.bsc.es%2Fgitlab)
[![Language](https://img.shields.io/static/v1?label=language&message=C99%20%2F%20C%2B%2B20&color=red)](https://en.wikipedia.org/wiki/C%2B%2B20)

<p><b>The storage coordinator for HPC clusters</b></p>

</div>

## Overview

Scord is an open-source storage coordination service designed for Linux HPC
clusters. As a storage coordination service, Scord provides facilities for
system administrators to easily model their storage hierarchies and expose them
to users in a consistent manner. Once the storage architecture is modelled,
Scord can efficiently orchestrate data transfers between the different
storage tiers in order to support complex HPC workloads using node-local
storage, burst buffers and ad-hoc storage systems.

> **ℹ️** **Important**  
> This software was partially supported by the EuroHPC-funded project ADMIRE  
   (Project ID: 956748, https://www.admire-eurohpc.eu).

## Building and installing

This section describes how to build and install Scord. Two main options
are basically supported out of the box:

- Automatically building both Scord and its dependencies using
  [Spack](https://github.com/spack/spack).
- Build and install Scord manually.

### Building scord and its dependencies with Spack

Scord and all its dependencies can be built using
[Spack](https://github.com/spack/spack). If you already have Spack, make sure
you have the latest release. If you use a clone of the Spack `develop`
branch, be sure to pull the latest changes.

#### Installing Spack

If you haven't already, install Spack with the following commands:

```shell
$ git clone -c feature.manyFiles=true https://github.com/spack/spack
```

This will create a directory called `spack` in your machine. Once you have
cloned Spack, we recommend sourcing the appropriate script for your shell.
This will add Spack to your `PATH` and enable the use of the `spack` command:

```shell
# For bash/zsh/sh
$ . spack/share/spack/setup-env.sh

# For tcsh/csh
$ source spack/share/spack/setup-env.csh

# For fish
$ . spack/share/spack/setup-env.fish
```

Since Scord is not yet available in the official Spack repositories, you need
to add the Scord Spack repository to the local Spack namespace in your
machine. To do so, download the
[`spack/` directory](https://storage.bsc.es/gitlab/eu/admire/io-scheduler/-/tree/main/spack)
located in the root of Scord's repository root to your machine (e.g. under
`~/projects/scord/spack`) and execute the following:

```shell
spack repo add ~/projects/scord/spack/
```

You should now be able to fetch information from the Scord package using
Spack:

```shell
spack info scord
```

If that worked, you are now ready to install Scord:

```shell
spack install scord
```

You can include or remove variants with Spack when a custom Scord
build is desired. The available variants are listed below:


| Variant | Command     | Default | Description                                                              |
|---------|-------------|---------|--------------------------------------------------------------------------|
| OFI     | `scord+ofi` | True    | Use [libfabric](https://github.com/ofiwg/libfabric) as transport library |
| UCX     | `scord+ucx` | False   | Use [ucx](https://github.com/openucx/ucx.git) as transport library       |


> ⚠️ **Attention**  
> The initial install could take a while as Spack will install build
> dependencies (autoconf, automake, m4, libtool, pkg-config, etc.) as well as
> any dependencies of dependencies (cmake, perl, etc.) if you don’t already
> have these dependencies installed through Spack.

After the installation completes, remember that you first need to load
Scord in order to use it:

```shell
spack load scord
```

### Building scord manually

If you prefer to build and install Scord from sources, you can also do so
but bear in mind that compiling and running Scord requires up-to-date
versions of various software packages that will need to be available in your
system. Also beware that using excessively old versions of these packages can
cause indirect errors that are very difficult to track down.

With that in mind, the following software packages are required to build Scord
and need to be available in the system:

- A C++ compiler that supports the C++20 standard, for example
[GCC](https://gcc.gnu.org)/[Clang](https://clang.llvm.org/) versions
8.0/6.0 or later.
- [CMake](https://cmake.org) 3.19 or later.
- pkg-config 0.29.1 (earlier versions will probably work but haven't been tested).
- [Margo](https://github.com/mochi-hpc/mochi-margo) version 0.9.8 and later, and its dependencies:
  - [Argobots](https://github.com/pmodels/argobots) version 1.1 or later.
  - [Mercury](https://github.com/mercury-hpc/mercury) version 2.0.1 or later.
    - [libfabric](https://github.com/ofiwg/libfabric) version 1.14.0rc3 and/or
[ucx](https://github.com/openucx/ucx.git) version 1.13.
  - [JSON-C](https://github.com/json-c/json-c) version 0.13.1.
- [Thallium](https://github.com/mochi-hpc/mochi-thallium) version 0.10.1 or
 later.
- [libconfig-dev] version 1.4.9 or later.
- [redis-plus-plus](https://github.com/sewenew/redis-plus-plus) version 1.3.
3 or later, and its dependencies:
  - [hiredis](https://github.com/redis/hiredis) version 0.14.1 or later.

The following libraries are also required by Scord, but will be
automatically downloaded and compiled by the project as part of the standard
build process.

- [{fmt}](https://fmt.dev/latest/index.html) version 8.0.1 or later.
- [spdlog](https://github.com/gabime/spdlog) version 1.9.2 or later.
- [Catch2](https://github.com/catchorg/Catch2) version 3.0.1 or later.
- [tl/expected](https://github.com/TartanLlama/expected).
- [Rapid YAML](https://github.com/biojppm/rapidyaml) version 0.5.0 and later.

> **ℹ️** **Important**  
Margo and Argobots use `pkg-config` to ensure they compile and link correctly
with all of their dependencies' libraries. When building Scord manually,
you'll need to appropriately set either the ``PKG_CONFIG_PATH`` environment
variable or the ``CMAKE_PREFIX_PATH`` CMake variable to the appropriate
installation paths where the ``.pc`` files for Argobots and Margo reside.

#### Building

You may either install the full sources or clone the repository directly. The
package relies on CMake to build the service, which requires you to do an
out-of-source build. Thus, once the Scord sources are located in an
appropriate `SCORD_SOURCES` directory (for example, `$HOME/scord`), you can
build the service by running the following commands:

```bash
# replace <SCORD_SOURCES> with the directory where you cloned/unpacked
# the project sources
cd <SCORD_SOURCES>
mkdir build
cd build
# replace <CMAKE_OPTIONS> with your desired options for scord (see below)
cmake <CMAKE_OPTIONS> ..
make
```

#### A more complex example:

The following CMake options can be used to configure how Scord is built:

- `SCORD_TRANSPORT_LIBRARY`: This option allows configuring the transport
  library used by the service. Currently, both `libfabric` and `ucx` are
  supported.
- `SCORD_TRANSPORT_PROTOCOL`: This option allows configuring the default
  transport protocol used by the service. The value provided here will be
  used to set the `transport_protocol` configuration option in the
  `${PREFIX}/etc/scord.conf` installed alongside the service.
- `SCORD_BIND_ADDRESS`: This option allows configuring the default address used
  to communicate with the service. The value provided here will be used to
  set the `bind_address` configuration option in the `${PREFIX}/etc/scord.conf`
  installed alongside the service.
- `SCORD_BIND_PORT`: This option allows configuring the default port used to
  communicate with the service. The value provided here will be used to set
  the `bind_port` configuration option in the `${PREFIX}/etc/scord.conf`
  installed alongside the service.
- `SCORD_BUILD_EXAMPLES`: This option instructs CMake to build the programs
  contained in the `examples` subdirectory.
- `SCORD_BUILD_TESTS`: This option instructs CMake to build the tests
  contained in the `tests` subdirectory.

Thus, let's assume that we want to build Scord with the following
configuration:

1. The service should use `libfabric` as the transport library.
2. The service should use `tcp` as the communication protocol.
3. The server should listen for RPCs on address `192.168.0.111` and on port
   `52000`.
4. Usage examples should be built.

Let's also assume that Scord sources are located in `$HOME/scord`, that
dependencies were installed in `/opt`, and that we also want to install
Scord in `/usr/local`. Taking into account all these requirements, Scord
can be built by running the following commands:

```bash
cd $HOME/scord
mkdir build && cd build
cmake -DCMAKE_PREFIX_PATH:STRING=/opt \
      -DCMAKE_INSTALL_PREFIX:STRING=/usr/local \
      -DSCORD_BUILD_EXAMPLES:BOOL=ON \
      -DSCORD_BUILD_TESTS:BOOL=ON \
      -DSCORD_TRANSPORT_LIBRARY=libfabric \
      -DSCORD_TRANSPORT_PROTOCOL=ofi+tcp \
      -DSCORD_BIND_ADDRESS=192.168.0.111 \
      -DSCORD_BIND_PORT=52000 \
    ..
make
```

#### Running tests

Tests are integrated in [CTest](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Testing%20With%20CMake%20and%20CTest.html), CMake's testing facility. Once built, the
tests can be run in parallel using the `ctest` command line tool:

```console
~/projects/scord/build $ ctest --parallel 4
Test project /home/amiranda/var/projects/scord/repo/build
    Start 1: Scenario: Error messages can be printed
1/1 Test #1: Scenario: Error messages can be printed ...   Passed    0.14 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.14 sec
```

#### Installing

Assuming that the `CMAKE_INSTALL_PREFIX` has been set (see previous step) and
that you have write permissions to the destination directory, Scord can be
installed by running the following command from the build directory:

```bash
make install
```

## Running the service

Once built, the service can be started with:

```bash
scord
```

## Testing the configuration

If you just want to test that everything works as it should, you can start
Scord in foreground mode and redirect its logging output to the console
with the following command:

```bash
scord -f --force-console
```

Which should produce output similar to the following:

```
[2021-11-19 10:30:30.010445] [scord] [131119] [info] ===================================
[2021-11-19 10:30:30.010476] [scord] [131119] [info] Starting scord daemon (pid 158733)
[2021-11-19 10:30:30.010480] [scord] [131119] [info] ===================================
[2021-11-19 10:30:30.010483] [scord] [131119] [info]
[2021-11-19 10:30:30.010486] [scord] [131119] [info] [[ Configuration ]]
[2021-11-19 10:30:30.010491] [scord] [131119] [info]   - running as daemon?: no
[2021-11-19 10:30:30.010530] [scord] [131119] [info]   - log file: "/usr/local/var/scord/scord.log"
[2021-11-19 10:30:30.010592] [scord] [131119] [info]   - log file maximum size: 16777216
[2021-11-19 10:30:30.010613] [scord] [131119] [info]   - pidfile: "/usr/local/var/run/scord/scord.pid"
[2021-11-19 10:30:30.010646] [scord] [131119] [info]   - port for remote requests: 52000
[2021-11-19 10:30:30.010650] [scord] [131119] [info]   - workers: 4
[2021-11-19 10:30:30.010653] [scord] [131119] [info]
[2021-11-19 10:30:30.010656] [scord] [131119] [info] [[ Starting up ]]
[2021-11-19 10:30:30.010658] [scord] [131119] [info]  * Installing signal handlers...
[2021-11-19 10:30:30.010853] [scord] [131119] [info]  * Creating RPC listener...
[2021-11-19 10:30:30.066151] [scord] [131119] [info]
[2021-11-19 10:30:30.066161] [scord] [131119] [info] [[ Start up successful, awaiting requests... ]]
```

Now we can use the `scord_ping` CLI program packaged with the service to
send a `ping` RPC to Scord:

```bash
scord_ping ofi+tcp://192.168.0.111:52000
```

And the server logs should update with an entry similar the following one:

```
[2021-11-19 12:58:35.882827] [scord] [154225] [info] remote_procedure::PING(noargs)
```

Once finished, the server can be stopped by pressing Ctrl+C:

```
^C
[2021-11-19 13:19:26.448229] [scord] [158735] [warning] A signal (SIGINT) occurred.
[2021-11-19 13:19:26.552131] [scord] [158733] [info] ===================================
[2021-11-19 13:19:26.552170] [scord] [158733] [info] Stopping scord daemon (pid 158733)
[2021-11-19 13:19:26.552176] [scord] [158733] [info] ===================================
[2021-11-19 13:19:26.552182] [scord] [158733] [info] * Stopping signal listener...
[2021-11-19 13:19:26.552280] [scord] [158733] [info]
[2021-11-19 13:19:26.552289] [scord] [158733] [info] [Stopped]
```
