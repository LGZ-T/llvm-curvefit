#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>

#define CPUT 0.37037
#define MAXBB 2000
#define FIRST 10
typedef unsigned long long ulonglong;

/** @return Hz **/
/*static unsigned long get_cpu_freq_by_sys()
{
   const char* file = "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq";
   FILE* f = fopen(file,"r");
   if(f==NULL){
      perror("Unable Find CPU freq sys file:");
      fprintf(stderr, "%s\n", file);
      return 0;
   }
   unsigned long freq = 0;
   fscanf(f, "%lu", &freq);
   if(freq==0)
      perror("Unable Read CPU freq:");
   fclose(f);
   return freq*1000;
}*/

/* We use 64bit values for the times.  */

/** copy code from simple-pmu:cycles.h (http://halobates.de/simple-pmu) **/
/*static inline ulonglong timing(void)
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

ulonglong bbcount[MAXBB];
long double bbtime[MAXBB];
ulonglong cycle1, cycle2;
int prebbid = 0;

void getBBTime1(ulonglong bbid)
{
    cycle1 = timing();
}

void getBBTime2(ulonglong bbid)
{
    cycle2 = timing();
    cycle2 -= cycle1;
    ++bbcount[bbid];
    bbtime[bbid] = ((bbcount[bbid]-1)*bbtime[bbid]+cycle2)/bbcount[bbid];
}*/

ulonglong BlockPredCount[MAXBB];
ulonglong BlockPredCycle[MAXBB];
void outinfo_bbtime()
{
    char outstring[150];
    char hostname[100];
    int pid = getpid(), i;
    long double t = 1.0;
    gethostname(hostname,99);
    sprintf(outstring,"%s.%d.bbtimeout",hostname,pid);
    FILE *fp = fopen(outstring,"w");
    if(fp==NULL) { printf("open file wrong\n"); return; }
    for(i=0;i<MAXBB;++i)
    {
        if(BlockPredCount[i]==0) BlockPredCount[i] = BlockPredCount[i-1];
        if(BlockPredCycle[i]==0) continue;
        fprintf(fp,"%d\t%Lf\t%llu\n",i,BlockPredCycle[i]/(t*BlockPredCount[i]),BlockPredCount[i]);    
    }
    fclose(fp);
}
void outinfo_bbcount()
{
    char outstring[150];
    char hostname[100];
    int pid = getpid(), i;
    gethostname(hostname,99);
    sprintf(outstring,"%s.%d.bbcountout",hostname,pid);
    FILE *fp = fopen(outstring,"w");
    if(fp==NULL) { printf("open file wrong\n"); return; }
    for(i=0;i<MAXBB;++i)
    {
        if(BlockPredCount[i]==0) continue;
        fprintf(fp,"%d\t%llu\n",i,BlockPredCount[i]);    
    }
    fclose(fp);
}

