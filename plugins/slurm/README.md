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

The core of the plugin is written in C and requires the Slurm and the Scord C
libraries to be compiled. It also requires access to the Slurm
`spank.h` header. The plugin is compiled as a shared object that is
loaded by Slurm's plugin system.

Scord's build system can be used to compile the plugin. The following
command will compile the plugin:

```bash
  $ make slurm-plugin install
```

Once the shared object is compiled, it only needs to be configured to be
used by Slurm. For example, if `libslurmadmcli.so` is installed in
`/usr/local/lib/`, the following line should be added to the Slurm plugin
configuration file (usually `/etc/slurm/plugstack.conf`)

```conf
  optional	/usr/local/lib/libslurmadmcli.so	scord_addr=ofi+tcp://127.0.0.1:52000 scord_proto=ofi+tcp://127.0.0.1 scordctl_bin=scord-ctl
```

The key-value pairs following the plugin are optional configuration
variables.

- `scord_addr`: The address to contact the Scord service in Mercury format.
  For instance, if the Scord service has been configured to listen on
  port `52000` on a machine with the IP address `192.168.1.128` and using
  `tcp` as the transport protocol, the address would be
  `ofi+tcp://192.168.1.128:52000`.
- `scord_proto`: The transport protocol to use in Mercury format (default
  to `ofi+tcp`). Must be the same configured in Scord (see Scord's
  documentation for more details).
- `scordctl_bin`: The scord-ctl binary to run on every node of an
  allocation, can be the path to an executable (default to scord-ctl).

Besides the shared library, the plugin also installs
[prolog and epilog](https://slurm.schedmd.com/prolog_epilog.html)
scripts for job control under `$PREFIX/share/scord/slurm/`. In order to enable
them, the following lines should be added to Slurm's configuration file
(where `$PREFIX` should be replaced with the path where Scord is installed):

```conf
  Prolog=$PREFIX/share/scord/slurm/scord_prolog.sh
  Epilog=$PREFIX/share/scord/slurm/scord_epilog.sh
```

## Usage

The plugin extends Slurm's command line arguments to allow users to request
the deployment of adhoc storage services for their jobs. The following
arguments are available for `srun`/`sbatch`:

- `--adm-adhoc`: The job requires an adhoc storage service.
  The following types are supported:
    - `gekkofs`: The job requires the GekkoFS adhoc file system.
    - `expand`: The job requires the Expand adhoc file system.
    - `hercules`: The job requires the Hercules adhoc file system.
    - `dataclay`: The job requires the dataClay adhoc object store.

- `--adm-adhoc-local`: The adhoc storage service will be deployed on the
  same nodes as the compute nodes, but the adhoc nodes will not be shared
  with the application. This is the default mode.
- `--adm-adhoc-overlap`: The adhoc storage service will be deployed on the same
  nodes as the compute nodes and its nodes will be shared with the application.
- `--adm-adhoc-remote <adhoc_id>`: The job will use a remote and independent
  remote adhoc storage service that must have been previously deployed with the
  `--adm-adhoc-exclusive` option. An identifier for that adhoc storage service
  must be provided as an argument.

Users can request and control the automatic deployment of a remote adhoc
storage service using the following `srun`/`sbatch` arguments:

- `--adm-adhoc-exclusive`: The job allocation will be used exclusively for an
  adhoc storage service.
- `--adm-adhoc-nodes`: The number of nodes to use for the adhoc storage
  service. The nodes will be allocated from the same partition as the
  compute nodes. This option is only valid when used with
  `--adm-adhoc-local` or `--adm-adhoc-overlap`.

Examples:

```bash
# Request a local GekkoFS adhoc storage service
$ sbatch --adm-adhoc gekkofs [--adm-adhoc-local] script.sh

# Request a local (overlapping) GekkoFS adhoc storage service
$ sbatch --adm-adhoc gekkofs --adm-adhoc-overlap script.sh

# Request an independent GekkoFS adhoc storage service
$ sbatch --adm-adhoc gekkofs --adm-adhoc-exclusive --adm-adhoc-nodes 10 --adm-adhoc-walltime 00:10:00 noop.sh
Submitted batch job 42
Will deploy adhoc storage 123456
$ sbatch --adm-adhoc-remote 123456 --dependency=after:42 script.sh
```

## References

1. See manual page spank(7) and <slurm/spank.h>
2. See https://storage.bsc.es/gitlab/eu/admire/io-scheduler
