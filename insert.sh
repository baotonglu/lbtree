#!/bin/bash

thread_num=(0 1 2 4 8 16 24 48)
for j in {1..6}
do
    rm -rf /mnt/pmem0/baotong/lbtree.data
    /usr/bin/hog-machine.sh numactl --cpunodebind=0 --membind=0 ./lbtree thread ${thread_num[$j]} mempool 100 nvmpool "no-pool" 200 bulkload 50000000 keygen-8B/dbg-k50k 0.7 insert 50000000 keygen-8B/dbg-insert500
done