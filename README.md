# scord

I/O Scheduler prototype for the ADMIRE EU project

## Software requirements

Compiling and running `scord` requires up-to-date versions of various software
packages. Beware that using excessively old versions of these packages can
cause indirect errors that are very difficult to track down.

The following software packages are required to build `scord` and need to be
available in the system:

- A C++ compiler that supports the C++17 standard, for example
[GCC](https://gcc.gnu.org)/[Clang](https://clang.llvm.org/) versions
8.0/5.0 or later.
- [CMake](https://cmake.org) 3.14 or later.
- pkg-config 0.29.1 (earlier versions will probably work but haven't been tested).
- [yaml-cpp](https://github.com/jbeder/yaml-cpp) version 0.6.2 and later.
- [Margo](https://github.com/mochi-hpc/mochi-margo) version 0.9.6 and later, and its dependencies:
  - [Argobots](https://github.com/pmodels/argobots) version 1.1 or later.
  - [Mercury](https://github.com/mercury-hpc/mercury) version 2.0.1 or later.
    - [libfabric](https://github.com/ofiwg/libfabric) version 1.14.0rc3 and/or
[ucx](https://github.com/openucx/ucx.git) version 1.13.
  - [JSON-C](https://github.com/json-c/json-c) version 0.13.1.

The following libraries are also required by `scord`, but will be downloaded
and compiled by the project as part of the standard build process.

- [{fmt}]() version 8.0.1 or later.
- [spdlog]() version 1.9.2 or later.

> **ℹ️** **Important**  
Margo and Argobots use `pkg-config` to ensure they compile and link correctly
with all of their dependencies' libraries. When building `scord` manually,
you'll need to appropriately set either the ``PKG_CONFIG_PATH`` environment
variable or the ``CMAKE_PREFIX_PATH`` CMake variable to the appropriate
installation paths where the ``.pc`` files for Argobots and Margo reside.

## Building

You may either install the full sources or clone the repository directly. The
package relies on CMake to build the service, which requires you to do an
out-of-source build. Thus, once the `scord` sources are located in an
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

### A more complex example:

The following CMake options can be used to configure how `scord` is built:

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
- `SCORD_BUILD_EXAMPLES`: This option instructs CMakes to build the programs
  contained in the `examples` subdirectory.

Thus, let's assume that we want to build `scord` with the following
configuration:

1. The service should use `libfabric` as the transport library.
2. The service should use `tcp` as the communication protocol.
3. The server should listen for RPCs on address `192.168.0.111` and on port
   `52000`.
5. Usage examples should be built.

Let's also assume that `scord` sources are located in `$HOME/scord`, that
dependencies were installed in `/opt`, and that we also want to install
`scord` in `/usr/local`. Taking into account all these requirements, `scord`
can be built by running the following commands:

```bash
cd $HOME/scord
mkdir build && cd build
cmake -DCMAKE_PREFIX_PATH:STRING=/opt \
      -DCMAKE_INSTALL_PREFIX:STRING=/usr/local \
      -DSCORD_BUILD_EXAMPLES:BOOL=ON \
      -DSCORD_TRANSPORT_LIBRARY=libfabric \
      -DSCORD_TRANSPORT_PROTOCOL=ofi+tcp \
      -DSCORD_BIND_ADDRESS=192.168.0.111 \
      -DSCORD_BIND_PORT=52000 \
    ..
make
```

## Installing

Assuming that the CMAKE_INSTALL_PREFIX has been set (see previous step) and
that you have write permissions to the destination directory, `scord` can be
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
`scord` in foreground mode and redirect its logging output to the console
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
Now we can use one of the example programs to send a `ping` RPC to `scord`:

```bash

cd $HOME/scord/build/examples
./ping ofi+tcp://192.168.0.111:52000
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
