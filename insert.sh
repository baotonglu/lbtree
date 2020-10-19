#!/bin/bash

rm -rf /mnt/pmem0/baotong/lbtree.data

/usr/bin/hog-machine.sh numactl --cpunodebind=0 --membind=0 ./lbtree thread 1 mempool 100 nvmpool "no-pool" 200 bulkload 50000000 keygen-8B/dbg-k50k 0.6 insert 50000000 keygen-8B/dbg-insert500
