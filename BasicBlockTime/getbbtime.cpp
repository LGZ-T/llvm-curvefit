#include <map>
#include <sstream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <mpi.h>
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
    void outinfo();
}
map<ulonglong,ulonglong> counter;
map<ulonglong,long double> bbtime;

void getBBTime(ulonglong bbid)
{
    static bool isentry = true;
    static ulonglong  totaltime = 0, cycly1,cycly2, temp;
    static long double curtime, test;
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
            itc = counter.insert(pair<ulonglong,ulonglong>(bbid,1)).first;
            bbtime.insert(pair<ulonglong,ulonglong>(bbid,0));
        }
        isentry = false;
        cycly1 = timing();
    }
    else
    {
        cycly2 = timing();
        test = cycly2;
        test -= cycly1;
        //cycly1 = timing_err();
        //temp = timing_err();
        //cycly1 = temp-cycly1;
        if(test < 0) { fprintf(stderr,"%llu is less than 0\n",bbid); cycly2 = 0; }
        else cycly2 -= cycly1;
        //curtime = cycly2*CPUT;
        //fprintf(stderr,"%llu %Lf\n",bbid,curtime);
        itb = bbtime.find(bbid);
        itb->second = (itb->second*(itc->second-1)+cycly2)/itc->second;
        //totaltime += curtime;
        //fprintf(stderr,"block:%llu cycle:%Lf iter:%llu\n",bbid,itb->second,itc->second);
        isentry = true;
    }
}

void outinfo()
{
    int id;
    MPI_Comm_rank(MPI_COMM_WORLD,&id);
    if(id==0)
    {
        ostringstream outstring;
        char hostname[100];
        int pid = getpid();
        gethostname(hostname,99);
        outstring << hostname << "." << pid << ".out";
        ofstream ofs(outstring.str(),ofstream::out);
        map<ulonglong,ulonglong>::iterator itc,end;
        for(itc=counter.begin(),end=counter.end();itc!=end;itc++)
        {
            ofs << itc->first << "\t" << itc->second*bbtime[itc->first] <<"\t"<<itc->second << "\n";
            //fprintf(stderr,"%llu\t%Lf\n",itc->first,itc->second*bbtime[itc->first]);
        }
    }
}
