#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>

using namespace std;
typedef struct reu
{
    unsigned long reuse;
    unsigned long num;
    reu(unsigned long re, unsigned long n)
    {
        reuse = re;
        num = n;
    }
}reuse_data;

typedef struct
{
    int start;
    int end;
}Range;


//the range are not for alldata, they are for the elements of alldata, which is also a vector 
/*void recurive_process(vector<vector<reuse_data*> &alldata,int start,int end)
void process_data(vector<vector<reuse_data*> *> &alldata)
{
    //first check the leading bins of each problem size
    vector<reuse_data*> *temp = alldata[0];
    unsigned long equal = (*temp)[0]->reuse;
    int start = 0;
    vector<Range> range;
    Range ranget;

    for(int s=0,e=alldata.size();s<e;s++)
        if((*alldata[s])[0]->reuse!=equal) break;

    if(s==e) start = 1;//all of the leading bins are equal

    for(int i=0,j=alldata.size();i<j;i++)
    {
        ranget.start = start;
        ranget.end = alldata[i]->size()-1;
        range.push_back(ranget);
    }
    recurive_process(alldata,start,alldata.size()-1);

}*/

void process_data(vector<list<reuse_data*> *> &alldata)
{
    //first check the leading bins of each problem size
    list<reuse_data*> *temp = alldata[0];
    unsigned long equal = temp->front()->reuse;
    int start = 0;
    vector<Range> range;
    Range ranget;

    for(int s=0,e=alldata.size();s<e;s++)
        if(alldata[s]->front()->reuse!=equal) break;

    if(s==e) start = 1;//all of the leading bins are equal

    for(int i=0,j=alldata.size();i<j;i++)
    {
        ranget.start = start;
        ranget.end = alldata[i]->size()-1;
        range.push_back(ranget);
    }
    recurive_process(alldata,start,alldata.size()-1);

}

int main()
{
    char filename[50];
    //ifstream in;
    FILE *in;
    //string s("reference:2104774");
    unsigned long ref = 2104774, reuse, num, sum;
    long double pos;
    vector<reuse_data *> reuse_all;

    for(int i=10;i<=30;i++)
    {
        sum = 0;
        ref = 2104774;
        sprintf(filename,"mrd10.10.%d",i);
        in = fopen(filename,"r");
        if(in==NULL) printf("it is null\n");

        while(1)
        {
            fscanf(in,"%*10c%lu%*7c%lu%*5c%lu%*c",&ref,&reuse,&num);
            if(ref!=2104774) break;
            //printf("%d\t%lu\t%lu\n",i,reuse,num);
            reuse_all.push_back(new reuse_data(reuse,num));
            sum += num;
        }
        pos = 0;
        for(int j=0,size=reuse_all.size();j<size;j++)
        {
            reuse_data * temp = reuse_all[j];
            cout << i << "\t" << temp->reuse << "\t" << pos << endl;
            pos += temp->num/(long double)sum;
            cout << i << "\t" << temp->reuse << "\t" << pos << endl;
            delete temp;
        }
        reuse_all.clear();
        fclose(in);
    }
}
