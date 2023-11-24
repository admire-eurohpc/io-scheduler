#!/usr/bin/env bash
echo "GEKKOFS Script Called"

# example of a script that can be called by the adhoc service
# [2023-11-23 09:37:32.583868] [scord-ctl] [2199567] [info] rpc => id: 0 name: "ADM_deploy_adhoc_storage" from: "ofi+tcp;ofi_rxm://127.0.0.1:52000" body: {uuid: "gekkofs-JR4ny5xHMhmlwh6KqThfYt71IaoR9cH5", type: ADM_ADHOC_STORAGE_GEKKOFS, resources: {nodes: [{hostname: "broadwell-001", type: regular}, {hostname: "broadwell-002", type: regular}, {hostname: "broadwell-003", type: regular}, {hostname: "broadwell-004", type: regular}]}}
# option: start --hosts "broadwell-001,broadwell-002,broadwell-003,broadwell-004" --workdir /tmp/gekkofs/gekkofs-JR4ny5xHMhmlwh6KqThfYt71IaoR9cH5 --datadir /tmp/gekkofs/gekkofs-JR4ny5xHMhmlwh6KqThfYt71IaoR9cH5/data --mountdir /tmp/gekkofs/gekkofs-JR4ny5xHMhmlwh6KqThfYt71IaoR9cH5/mnt

# code to count the number of elements in a comma separated list environment variable (called $nodes) and store in $num_nodes

if ($1 == "start") then
    echo "Starting GEKKOFS"
    $nodes = $3
    num_nodes=$(echo $nodes | awk -F, '{print NF-1}')
    # If num_nodes is >40, we are on the testing environment
    if ($num_nodes > 40) then
        exit 0
    end
    $workdir = $5
    $datadir = $7
    $mountdir = $9

    mkdir -p $5
    srun -N $num_nodes -n $num_nodes --oversubscribe --cpus-per-task=1 --mem-per-task=1 --export=ALL bash -c "gkfs_daemon --rootdir $datadir --mountdir $mountdir" & 
else if ($1 == "stop") then
    echo "Stopping GEKKOFS"
    srun -N $num_nodes -n $num_nodes --oversubscribe --cpus-per-task=1 --mem-per-task=1 --export=ALL bash -c "pkill -9 gkfs_daemon"
else
    echo "Unknown command"
    exit 1


exit 0
