#!/usr/bin/bash
echo "GEKKOFS Script Called" $HOSTNAME $SLURM_JOBID


if [ "$1" == "start" ]; then
    echo "Starting GEKKOFS"
    . /beegfs/home/r.nou/spack/share/spack/setup-env.sh
    spack load gekkofs
    spack load slurm@23.02.6
    nodes=$3
    num_nodes=$(echo $nodes | awk -F, '{print NF-1}')
    # If num_nodes is greater than 40, we are on the testing environment
    if [ $num_nodes -gt 40 ]; then
        exit 0
    fi
    workdir=$5
    datadir=$7
    mountdir=$9

    mkdir -p $workdir
    /opt/slurm/bin/srun -N $num_nodes -n $num_nodes --oversubscribe --cpus-per-task=1 --mem-per-cpu=1 --export=ALL bash -c "gkfs_daemon --rootdir $datadir --mountdir $mountdir" &
   sleep 2
elif [ "$1" == "stop" ]; then
    echo "Stopping GEKKOFS"
    . /beegfs/home/r.nou/spack/share/spack/setup-env.sh
    spack load gekkofs
    spack load slurm@23.02.6
    /opt/slurm/bin/srun -N $num_nodes -n $num_nodes --oversubscribe --cpus-per-task=1 --mem-per-task=1 pkill -9 gkfs_daemon
else
    echo "Unknown command"
    exit 1
fi

exit 0
