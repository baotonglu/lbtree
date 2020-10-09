#!/bin/bash

rm -rf /mnt/pmem0/baotong/lbtree.data

/usr/bin/hog-machine.sh ./lbtree thread 1 mempool 100 nvmpool "no-pool" 200 bulkload 50000000 keygen-8B/dbg-k50k 1.0 lookup 50000000 keygen-8B/dbg-search500
