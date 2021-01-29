/**
 * @file nvmlog-common.h
 * @author  Shimin Chen <shimin.chen@gmail.com>, Jihang Liu, Leying Chen
 * @version 1.0
 *
 * @section LICENSE
 * 
 * TBD
 *       
 * @section DESCRIPTION
 *
 * This header defines constants and useful macros for NVM
 */

#ifndef _NVM_COMMON_H
#define _NVM_COMMON_H

/* -------------------------------------------------------------- */
#include "nodepref.h"

/* -------------------------------------------------------------- */
// must define one of the following:
// NVMFLUSH_REAL:  flush
// NVMFLUSH_STAT:  no flush, count stats
// NVMFLUSH_DUMMY: no flush

#define NVMFLUSH_REAL

#if   defined(NVMFLUSH_REAL)
#undef NVMFLUSH_DUMMY
#undef NVMFLUSH_STAT

#elif defined(NVMFLUSH_STAT)
#undef NVMFLUSH_DUMMY
#undef NVMFLUSH_REAL

#elif defined(NVMFLUSH_DUMMY)
#undef NVMFLUSH_STAT
#undef NVMFLUSH_REAL

#else
#error "must define NVMFLUSH_DUMMY, or NVMFLUSH_STAT, or NVMFLUSH_REAL"
#endif


/* -------------------------------------------------------------- */
#if   defined(NVMFLUSH_REAL)
/* -------------------------------------------------------------- */
// use clwb and sfence

/**
 * flush a cache line
 *
 * @param addr   the address of the cache line
 */
static inline
void clwb(void * addr)
{ asm volatile("clwb %0": :"m"(*((char *)addr))); } 

/**
 * flush [start, end]
 *
 * there are at most two lines.
 */
static inline
void clwb2(void *start, void *end)
{
  clwb(start);
  if (GET_LINE(start) != GET_LINE(end)) {
    clwb(end);
  }
}

/**
 * flush [start, end]
 *
 * there can be 1 to many lines
 */
static inline
void clwbmore(void *start, void *end)
{ 
  unsigned long long start_line= GET_LINE(start);
  unsigned long long end_line= GET_LINE(end);
  do {
    clwb((char *)start_line);
    start_line += CACHE_LINE_SIZE;
  } while (start_line <= end_line);
}

/**
 * call sfence
 */
static inline
void sfence(void)
{ asm volatile("sfence"); }

/* -------------------------------------------------------------- */
#elif defined(NVMFLUSH_STAT)
/* -------------------------------------------------------------- */

#define NVMFLUSH_STAT_DEFS   long long num_clwb, num_sfence

static inline
void NVMFLUSH_STAT_init()
{
    num_clwb=0;
    num_sfence= 0;
}

static inline
void NVMFLUSH_STAT_print()
{
    printf("num_clwb=%lld, num_sfence=%lld\n", num_clwb, num_sfence);
}

static inline
void clwb(void * addr)
{
   num_clwb ++;
 //printf("clwb(%p)\n", addr);
}

static inline
void clwb2(void *start, void *end)
{
  num_clwb ++;
  if (GET_LINE(start) != GET_LINE(end)) {
    num_clwb ++;
  }
  // printf("clwb2(%p, %p)\n", start, end);
}

static inline
void clwbmore(void *start, void *end)
{
  unsigned long long start_line= GET_LINE(start);
  unsigned long long end_line= GET_LINE(end);
  num_clwb += (end_line+CACHE_LINE_SIZE-start_line)/CACHE_LINE_SIZE;

  // printf("clwbmore(%p, %p)\n", start, end);
}

static inline
void sfence(void)
{
   num_sfence ++;
 //printf("sfence()\n");
}

/* -------------------------------------------------------------- */
#elif defined(NVMFLUSH_DUMMY)
/* -------------------------------------------------------------- */

static inline void clwb(void * addr) {}
static inline void clwb2(void *start, void *end) {}
static inline void clwbmore(void *start, void *end) {}
static inline void sfence(void) {}

#endif  // NVMFLUSH_DUMMY

/* -------------------------------------------------------------- */

#define LOOP_FLUSH(cmd, ptr, nline)               \
{char *_p= (char *)(ptr);                         \
 char *_pend= _p + (nline)*CACHE_LINE_SIZE;       \
 for (; _p<_pend; _p+=(CACHE_LINE_SIZE)) cmd(_p); \
}

/* -------------------------------------------------------------- */
/**
 * copy src line to dest line using movntdq
 *
 * Both src and dest are 64 byte lines and aligned
 *
 * @param dest  destination line
 * @param src   src line
 */
static inline
void writeLineMOVNT(char *dest, char *src)
{   
  asm volatile(
    "movdqa   %4, %%xmm0    \n\t"
    "movdqa   %5, %%xmm1    \n\t"
    "movdqa   %6, %%xmm2    \n\t"
    "movdqa   %7, %%xmm3    \n\t"
    "movntdq  %%xmm0, %0    \n\t"
    "movntdq  %%xmm1, %1    \n\t"
    "movntdq  %%xmm2, %2    \n\t"
    "movntdq  %%xmm3, %3    \n\t" 
    : "=m"(dest[ 0]), "=m"(dest[16]), "=m"(dest[32]), "=m"(dest[48])
    :  "m"(src[ 0]),   "m"(src[16]),   "m"(src[32]),   "m"(src[48])
    : "xmm0", "xmm1", "xmm2", "xmm3",
      "memory");
}

/* -------------------------------------------------------------- */
// Log is kept for every thread
#ifndef NVM_LOG_SIZE
#define NVM_LOG_SIZE	   (1*MB)
#endif

/* -------------------------------------------------------------- */
// Tags
#define NL_INVALID       0x0

#define NL_NEW1B         0x1
#define NL_NEW2B         0x2
#define NL_NEW4B         0x3
#define NL_NEW8B         0x4
#define NL_NEWVCHAR      0x5

#define NL_WRITE1B       0x6
#define NL_WRITE2B       0x7
#define NL_WRITE4B       0x8
#define NL_WRITE8B       0x9
#define NL_WRITEVCHAR    0xa

#define NL_REDO1B        0xb
#define NL_REDO2B        0xc
#define NL_REDO4B        0xd
#define NL_REDO8B        0xe
#define NL_REDOVCHAR     0xf

#define NL_ALLOCNODE     0x10
#define NL_DELNODE       0x11

#define NL_NEXT_CHUNK    0x20  // go to the next chunk

#define NL_COMMIT        0x80
#define NL_ABORT         0x81
#define NL_ONGOING       0x82

#endif /* _NVM_COMMON_H */
