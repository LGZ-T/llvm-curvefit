#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
typedef struct
{
    unsigned long long reuse;
    unsigned long long num;
}reuse_data;

//vector<vector<reuse_data *> *> result;

int main()
{
    char filename[50];
    //ifstream in;
    FILE *in;
    //string s("reference:2104774");
    unsigned long long ref = 2104774, reuse, num;
    vector<reuse_data *> reuse_all;

    for(int i=10;i<=30;i++)
    {
        reuse_all.push_back(reuse_all);
        sprintf(filename,"mrd10.10.%d",i);
        in = fopen(filename,"r");
        if(in==NULL) printf("it is null\n");
        //printf("file:%s\n",filename);

        while(1)
        {
            fscanf(in,"%*10c%llu%*7c%llu%*5c%llu%*c",&ref,&reuse,&num);
            if(ref!=2104774) break;
            printf("%d\t%llu\t%llu\n",i,reuse,num);

        }

        fclose(in);
        ref = 2104774;
    }
}
