//#include <iostream>
#include <map>
#include <string>
//#include <time.h>
//#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#define CPUT 0.37037

using namespace std;
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


/*long double timing_res() 
{
   long double freq = get_cpu_freq_by_sys();
   printf("freq=%Lf\n",freq);
   return 1E9 / freq;
}*/
uint64_t timing_err()
{
    static ulonglong a = 0;
    static bool first = true;
    if(first)
    {
        first = false;
        a=timing();
    }
    else
    {
        a=timing();
        first = true;
    }
    return a;
}

extern "C"
{
    void getBBTime(ulonglong bbid);
}
map<ulonglong,ulonglong> counter;
map<ulonglong,long double> bbtime;

void getBBTime(ulonglong bbid)
{
    static bool isentry = true;
    static ulonglong  totaltime = 0, cycly1,cycly2, temp;
    static long double curtime;
    static map<ulonglong,ulonglong>::iterator itc;
    static map<ulonglong,long double>::iterator itb;
    if(isentry)
    {
        itc = counter.find(bbid);
        if(itc!=counter.end())
        {
            itc->second++;
        }
        else
        {
            counter.insert(pair<ulonglong,ulonglong>(bbid,1));
            bbtime.insert(pair<ulonglong,long double>(bbid,0));
        }
        isentry = false;
        cycly1 = timing();
    }
    else
    {
        cycly2 = timing();
        cycly2 -= cycly1;
        cycly1 = timing_err();
        temp = timing_err();
        cycly1 = temp-cycly1;
        if(cycly2<cycly1) cycly2 = 0;
        else cycly2 -= cycly1;
        curtime = cycly2*CPUT;  //timing_res();
        //fprintf(stderr,"%llu %Lf\n",bbid,curtime);
        itb = bbtime.find(bbid);
        itb->second = (itb->second*(itc->second-1)+curtime)/itc->second;
        totaltime += curtime;
        isentry = true;
    }
}
void outinfo()
{
    map<ulonglong,ulonglong>::iterator itc,end;
    for(itc=counter.begin(),end=counter.end();itc!=end;itc++)
    {
        fprintf(stderr,"%llu\t%Lf\n",itc->first,itc->second*bbtime[itc->first]);
    }
}
