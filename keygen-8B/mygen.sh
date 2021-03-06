#!/bin/bash

# ----------------------------------------------------------------------
# the following is for debugging
# bulkload keys must be sorted
./keygen 100000000 sort dbg-k50k

# search keys are randomly ordered
./keygen 100000000 random dbg-search500

# insert keys are randomly ordered and are not in bulkload keys
./getinsert 100000000 dbg-k50k 250000000 dbg-insert500

# delete keys are randomly ordered and must be in bulkload keys
./getdelete 100000000 dbg-k50k 100000000 dbg-delete500


# ----------------------------------------------------------------------
# the following is for experiments
# bulkload keys must be sorted
#./keygen 50000000 sort k50m

# search keys are randomly ordered
#./keygen 500000 random search500k

# insert keys are randomly ordered and are not in bulkload keys
#./getinsert 50000000 k50m 500000 insert500k

# delete keys are randomly ordered and must be in bulkload keys
#./getdelete 50000000 k50m 500000 delete500k
