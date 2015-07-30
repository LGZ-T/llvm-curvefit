#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

typedef unsigned long long ulonglong;

/* We use 64bit values for the times.  */

/** copy code from simple-pmu:cycles.h (http://halobates.de/simple-pmu) **/
static inline ulonglong timing(void)
{
#ifdef __i386__
	unsigned long long s;
	asm volatile("rdtsc" : "=A" (s) :: "memory");
	return s;
#else
	unsigned low, high;
	asm volatile("rdtsc" : "=a" (low), "=d" (high) :: "memory");
	return ((unsigned long long)high << 32) | low;
#endif
}


uint64_t timing_err()
{
    static ulonglong a = 0,b = 0;
    a=timing();
    b=timing();
    return b-a;
}

/*extern "C"
{
    void getBBTime1();
    void getBBTime2();
}*/

ulonglong first,second;

void getfuncTime1()
{
    first = timing();
}

void getfuncTime2()
{
        second = timing();
        second = second - first;
        ulonglong temp = timing_err();
        if(second<temp) second = 3;
        else second -= temp;
        static int pid = -1;
        if(pid==-1) pid = getpid();
        fprintf(stderr,"%d %llu\n",pid,second);
}

