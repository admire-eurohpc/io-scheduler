# Slurm plugin

This directory contains a Slurm plugin that enables the use of the
Scord storage coordination service in Slurm jobs. The plugin consists
of a shared object (`libslurmadmcli.so`) that is loaded by Slurm's plugin
system and extends Slurm's functionalities.

The `libslurmadmcli.so` shared library is a Slurm SPANK¹ plugin that extends
Slurm's command line arguments to allow users to provide information for
`scord`, serving as a simple interface between Slurm itself and the Scord
storage coordination service².

## Installation

The plugin is written in C and requires the Slurm and the Scord C
libraries to be compiled. It also requires access to the Slurm
`spank.h` header. The plugin is compiled as a shared object that is
loaded by Slurm's plugin system.

Scord's build system can be used to compile the plugin. The following
command will compile the plugin:

```bash
  $ make slurm-plugin
```

Once the shared object is compiled, it only needs to be copied
somewhere the Slurm plugin system can pick it up. For example, if
`libslurmadmcli.so` is installed in `/usr/local/lib/`, the following line
should be added to the Slurm plugin configuration file (usually
`/etc/slurm/plugstack.conf`)

```conf
  optional	/usr/local/lib/libslurmadmcli.so	scord_addr=ofi+tcp://127.0.0.1:52000 scord_proto=ofi+tcp://127.0.0.1 scordctl_bin=scord-ctl
```

The key-value pairs following the plugin are optional configuration
variables.

- `scord_addr`: The address to contact the Scord service in Mercury format
  (default to ofi+tcp://127.0.0.1:52000)
- `scord_proto`: The transport protocol to use in Mercury format (default
  to ofi+tcp).
- `scordctl_bin`: The scord-ctl binary to run on every node of an
  allocation, can be the path to an executable (default to scord-ctl).

## References

1. See manual page spank(7) and <slurm/spank.h>
2. See https://storage.bsc.es/gitlab/eu/admire/io-scheduler
