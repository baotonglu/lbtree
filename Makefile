CC=g++

# Flag for debugging runs
# CFLAGS=-O0 -g -std=c++11 -pthread -mrtm -msse4.1 -mavx2

# Flag for test runs
CFLAGS=-O3 -std=c++11 -pthread -mrtm -msse4.1 -mavx2 

INCLUDE=-I./common
LIB=-lpmem -lpmemobj

COMMON_DEPENDS= ./common/tree.h ./common/mytree.h ./common/keyinput.h ./common/mempool.h ./common/mempool.cc ./common/nodepref.h ./common/nvm-common.h ./common/nvm-common.cc ./common/performance.h ./common/allocator.h
COMMON_SOURCES= ./common/mempool.cc ./common/nvm-common.cc

# -----------------------------------------------------------------------------
TARGETS=lbtree 

#wbtree fptree

all: ${TARGETS}

# -----------------------------------------------------------------------------

lbtree: lbtree-src/lbtree.h lbtree-src/lbtree.cc ${COMMON_DEPENDS}
	${CC} -o $@ ${CFLAGS} ${INCLUDE} lbtree-src/lbtree.cc ${COMMON_SOURCES} ${LIB}


# -----------------------------------------------------------------------------
clean:
	-rm -rf a.out core *.s ${TARGETS}
