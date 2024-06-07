#!/usr/bin/bash
echo "GEKKOFS Script Called" $HOSTNAME $SLURM_JOBID
# If GKFS_DAEMON is not defined then define it here
if [ -z "$GKFS_DAEMON" ]; then
    GKFS_DAEMON=/home/rnou/iodeps/bin/gkfs_daemon
fi
# If LIBGKFS_HOSTS_FILE is not defined then define it here
if [ -z "$LIBGKFS_HOSTS_FILE" ]; then
    LIBGKFS_HOSTS_FILE=/tmp/gekkofs/gkfs_hosts.txt
fi

if [ -z "$GKFS_CLIENT" ]; then
    GKFS_CLIENT=/home/rnou/iodeps/lib/libgkfs_intercept.so
fi

if [ "$1" == "start" ]; then
    echo "Starting GEKKOFS"

    nodes=$3
    num_nodes=$(echo $nodes | awk -F, '{print NF}')
    # If num_nodes is 50, we are on the testing environment
    if [ $num_nodes -eq 50 ]; then
        exit 0
    fi
    workdir=$5
    datadir=$7
    mountdir=$9
    unset SLURM_CPU_BIND SLURM_CPU_BIND_LIST SLURM_CPU_BIND_TYPE SLURM_CPU_BIND_VERBOSE
    
    srun -N $num_nodes -n $num_nodes --oversubscribe --overlap --cpus-per-task=1 --mem-per-cpu=1 --export=ALL /usr/bin/bash -c "mkdir -p $mountdir; mkdir -p $datadir" 
    srun -N $num_nodes -n $num_nodes --oversubscribe --overlap --cpus-per-task=1 --mem-per-cpu=1 --export=ALL /usr/bin/bash -c "$GKFS_DAEMON --rootdir $datadir --mountdir $mountdir -H $LIBGKFS_HOSTS_FILE" &
    sleep 4
    echo "Started GEKKOFS"
elif [ "$1" == "stop" ]; then
    echo "Stopping GEKKOFS"
    
    nodes=$3
    num_nodes=$(echo $nodes | awk -F, '{print NF}')
    # If num_nodes is 50, we are on the testing environment
    if [ $num_nodes -eq 50 ]; then
        exit 0
    fi
    unset SLURM_CPU_BIND SLURM_CPU_BIND_LIST SLURM_CPU_BIND_TYPE SLURM_CPU_BIND_VERBOSE
    srun -N $num_nodes -n $num_nodes --overlap --oversubscribe --cpus-per-task=1 --mem-per-cpu=1 --export=ALL /usr/bin/bash -c "pkill -9 gkfs_daemon"
elif [ "$1" == "expand" ]; then
    echo "Expand command"
elif [ "$1" == "shrink" ]; then
    echo "shrink command"
fi

exit 0
