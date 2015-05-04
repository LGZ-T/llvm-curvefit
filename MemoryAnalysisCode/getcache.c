#include <stdio.h>

#define L1SIZE 16*1024
#define L2SIZE 1024*1024

int main()
{
    char filename[20];
    FILE *in;
    unsigned long reuse=0, num=0, hitl1=0,hitl2=0,hitmem=0;
    //for(int i=10;i<=30;i++)//there are 30 files
    //{
        sprintf(filename,"mrd10.10.10");
        in = fopen(filename,"r");
        if(in==NULL) printf("it is null\n");

        /*fscanf(in,"%*10c%*lu%*7c%lu%*5c%lu%*c",&reuse,&num);
        if(reuse<=L1SIZE) hitl1 += num;
        else if(reuse<=L2SIZE) hitl2 += num;
        else hitmem += num;*/
        fscanf(in,"%*10c%*lu%*7c%lu%*5c%lu%*c",&reuse,&num);
        while(!feof(in))
        {
            printf("reuse is:%lu\n",reuse);
            if(reuse<=L1SIZE) hitl1 += num;
            else if(reuse<=L2SIZE) hitl2 += num;
            else hitmem += num;
            fscanf(in,"%*10c%*lu%*7c%lu%*5c%lu%*c",&reuse,&num);
        }
        fclose(in);
    //}
    printf("HITL1:%lu\n",hitl1);
    printf("HITL2:%lu\n",hitl2);
    printf("HITMM:%lu\n",hitmem);
    return 0;
}
