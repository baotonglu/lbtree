#!/bin/bash

rm -rf /mnt/pmem0/baotong/lbtree.data

/usr/bin/hog-machine.sh ./lbtree thread 1 mempool 100 nvmpool "no-pool" 200 bulkload 100000000 keygen-8B/dbg-k50k 0.6 lookup 100000000 keygen-8B/dbg-delete500
