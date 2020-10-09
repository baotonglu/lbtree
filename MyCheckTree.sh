#!/bin/bash

if [ $# != 1 ]; then
  echo "Usage: $0 <tree>";
  exit 0;
fi

if [ ! -x $1 ]; then
  echo "$1 is not an executable";
  exit 0;
fi

nvmfile=/mnt/mypmem0/chensm/leafdata

cmdinit="$1 thread 2 mempool 50 nvmpool ${nvmfile} 200"

echo 'debug_bulkload'
echo -n 'Test  1: '
rm -rf /mnt/pmem0/baotong/lbtree.data
/usr/bin/hog-machine.sh ${cmdinit} debug_bulkload 123 1.0 | grep good
echo -n 'Test  2: '
rm -rf /mnt/pmem0/baotong/lbtree.data
${cmdinit} debug_bulkload 500 0.9 | grep good
echo -n 'Test  3: '
rm -rf /mnt/pmem0/baotong/lbtree.data
${cmdinit} debug_bulkload 1234567 1.0 | grep good
echo -n 'Test  4: '
rm -rf /mnt/pmem0/baotong/lbtree.data
${cmdinit} debug_bulkload 1234567 0.7 | grep good

echo 'debug_insert'
echo -n 'Test  5: '
rm -rf /mnt/pmem0/baotong/lbtree.data
${cmdinit} debug_insert 10 | grep good
echo -n 'Test  6: '
rm -rf /mnt/pmem0/baotong/lbtree.data
${cmdinit} debug_insert 102 | grep good
echo -n 'Test  7: '
rm -rf /mnt/pmem0/baotong/lbtree.data
${cmdinit} debug_insert 1025 | grep good
echo -n 'Test  8: '
rm -rf /mnt/pmem0/baotong/lbtree.data
${cmdinit} debug_insert 31025 | grep good
echo -n 'Test  9: '
rm -rf /mnt/pmem0/baotong/lbtree.data
${cmdinit} debug_insert 371025 | grep good

echo 'debug_del'
echo -n 'Test 10: '
rm -rf /mnt/pmem0/baotong/lbtree.data
${cmdinit} debug_del 10 | grep good
echo -n 'Test 11: '
rm -rf /mnt/pmem0/baotong/lbtree.data
${cmdinit} debug_del 120 | grep good
echo -n 'Test 12: '
rm -rf /mnt/pmem0/baotong/lbtree.data
${cmdinit} debug_del 3120 | grep good
echo -n 'Test 13: '
rm -rf /mnt/pmem0/baotong/lbtree.data
${cmdinit} debug_del 83120 | grep good
echo -n 'Test 14: '
rm -rf /mnt/pmem0/baotong/lbtree.data
${cmdinit} debug_del 583120 | grep good

echo 'debug_lookup'
echo -n 'Test 15: '
rm -rf /mnt/pmem0/baotong/lbtree.data
${cmdinit} debug_lookup 20 1.0 | grep good
echo -n 'Test 16: '
rm -rf /mnt/pmem0/baotong/lbtree.data
${cmdinit} debug_lookup 320 1.0 | grep good
echo -n 'Test 17: '
rm -rf /mnt/pmem0/baotong/lbtree.data
${cmdinit} debug_lookup 5320 1.0 | grep good
echo -n 'Test 18: '
rm -rf /mnt/pmem0/baotong/lbtree.data
${cmdinit} debug_lookup 101180 1.0 | grep good
