#!/usr/bin/bash
echo "GEKKOFS Script Called" $HOSTNAME $SLURM_JOBID


if [ "$1" == "start" ]; then
    echo "Starting GEKKOFS"

    nodes=$3
    num_nodes=$(echo $nodes | awk -F, '{print NF}')
    # If num_nodes is greater than 40, we are on the testing environment
    if [ $num_nodes -gt 40 ]; then
        exit 0
    fi
    workdir=$5
    datadir=$7
    mountdir=$9
    unset SLURM_CPU_BIND SLURM_CPU_BIND_LIST SLURM_CPU_BIND_TYPE SLURM_CPU_BIND_VERBOSE
    
    srun -N $num_nodes -n $num_nodes --oversubscribe --cpus-per-task=1 --mem-per-cpu=1 --export=ALL bash -c "gkfs_daemon --rootdir $datadir --mountdir $mountdir" &
    sleep 4
elif [ "$1" == "stop" ]; then
    echo "Stopping GEKKOFS"
    srun -N $num_nodes -n $num_nodes --oversubscribe --cpus-per-task=1 --mem-per-task=1 pkill -9 gkfs_daemon
elif [ "$1" == "expand" ]; then
    echo "Expand command"
elif [ "$1" == "shrink" ]; then
    echo "shrink command"
fi

exit 0
