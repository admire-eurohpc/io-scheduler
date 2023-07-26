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

### Ad-hoc storage services

The plugin extends Slurm's command line arguments to allow users to request
the deployment of adhoc storage services for their jobs. The following
arguments are available for `srun`/`sbatch`:

- `--adm-adhoc`: The job requires an adhoc storage service. By default
  `--adm-adhoc-overlap` is assumed. The type of adhoc storage service can be
  one of:
    - `gekkofs`: The job requires the GekkoFS adhoc file system.
    - `expand`: The job requires the Expand adhoc file system.
    - `hercules`: The job requires the Hercules adhoc file system.
    - `dataclay`: The job requires the dataClay adhoc object store.

- `--adm-adhoc-overlap`: The requested adhoc storage service will be
  deployed on the same nodes as the application and the nodes will be
  shared. The number of nodes assigned for the adhoc storage service can
  be controlled with the `--adm-adhoc-nodes` option. If not specified, the
  deployed adhoc storage service will share all the nodes assigned to the job.

- `--adm-adhoc-exclusive`: The adhoc storage service will be deployed on the
  same nodes as the application, but the adhoc nodes will not
  be shared with the application. The number of nodes assigned for the adhoc
  storage service MUST be specified with the `--adm-adhoc-nodes` option and
  cannot be greater than the number of nodes assigned to the job. Note, however,
  that the value of `--adm-adhoc-nodes` must be smaller than the value of
  `--nodes` (or `--ntasks`). Otherwise, the application would have no
  resources to run on.

- `--adm-adhoc-dedicated`: The ad-hoc storage service will be deployed in an
  independent job allocation and all the nodes for the allocation will be
  available for it. An `adhoc-id` will be generated for it and will be
  returned to the user so that other jobs can use the deployed ad-hoc storage
  service. In this mode, the resources assigned to the ad-hoc storage service
  can be controlled with the usual Slurm options (e.g. `--nodes`,
  `--ntasks`, `--time`, etc.).

- `--adm-adhoc-remote <adhoc-id>`: The job will use a remote and
  dedicated ad-hoc storage service that must have been previously requested in a
  different submission with the `--adm-adhoc-dedicated` option. An
  identifier for that ad-hoc storage service must be provided as an argument.

Users can request and control the automatic deployment of a remote adhoc
storage service using the following `srun`/`sbatch` arguments:

- `--adm-adhoc-dedicated`: The job allocation will be used exclusively for an
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
# Wait for the adhoc storage service to be started
$ sbatch --adm-adhoc-remote 123456 --dependency=after:42 script.sh
```

### Dataset management

The plugin also provides a set of options to manage datasets:

- `--adm-input <dataset-routing>`: Define datasets that should be transferred
  between the PFS and the ad-hoc storage service. The `dataset-routing` is
  defined as `ORIGIN-TIER:PATH TARGET-TIER:PATH`. For example, to transfer
  the file `input000.dat` from the Lustre PFS to the an on-demand GekkoFS
  ad-hoc storage service, the option could be specified in the following manner:
  `"lustre:/input.dat gekkofs:/input.dat"`
- `--adm-output <dataset-routing>`: Define datasets that should be
  automatically transferred between the ad-hoc storage system and the PFS.
  The ad-hoc storage will guarantee that the dataset is not transferred while
  there are processes accessing the file. The datasets will be transferred
  before the job allocation finishes if at all possible, but no hard guarantees
  are made.
- `--adm-expect-output <dataset-routing>`: Define datasets that should be
  automatically transferred between the ad-hoc storage system and the PFS.
  The ad-hoc storage will guarantee that the dataset is not transferred while
  there are processes accessing the file. The datasets will be transferred
  before the job allocation finishes. If the transfer cannot be completed
  before the job allocation finishes, the job will be cancelled.
- `--adm-inout <dataset-routing>`: Define datasets that should be
  transferred INTO the ad-hoc storage AND BACK when finished.

## References

1. See manual page spank(7) and <slurm/spank.h>
2. See https://storage.bsc.es/gitlab/eu/admire/io-scheduler
