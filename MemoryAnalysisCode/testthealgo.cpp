#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>

using namespace std;
typedef unsigned long ulong;

typedef struct reu
{
    ulong reuse;
    ulong num;
    ulong sum_num;
    struct reu * next;
    reu(ulong re, ulong n, ulong s)
    {
        reuse = re;
        num = n;
        sum_num = s;
    }
}reuse_data;

class module
{
    ulong ave_reuse, sum_num;
    reuse_data *start, *end;
    module *left, *right, *up, *down;

    void append(reuse_data *);
    module * split(double);
};

/*all of these are old code,i am not sure if they are useful, so, i just keep it. ^_^ just in case.
typedef struct
{
    int start;
    int end;
}Range;

//the range are not for alldata, they are for the elements of alldata, which is also a vector 
void recurive_process(vector<vector<reuse_data*> &alldata,int start,int end)
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

}

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

}*/


int main()
{
    char filename[50];
    FILE *in;
    unsigned long ref = 2104774, reuse, num, sum;
    long double pos;
    
    //a reference's histogram data is stored in reu_for_one_ref
    //all data of a reference is stored in reu_for_all_ref
    vector<reuse_data *> *reu_for_one_ref;
    vector<vector<reuse_data *> *> reu_for_all_ref;

    for(int i=10;i<=30;i++)//there are 30 files
    {
        sum = 0;
        ref = 2104774;
        sprintf(filename,"mrd10.10.%d",i);
        in = fopen(filename,"r");
        if(in==NULL) printf("it is null\n");
        
        reu_for_one_ref = new vector<reuse_data *>();

        while(1)
        {
            fscanf(in,"%*10c%lu%*7c%lu%*5c%lu%*c",&ref,&reuse,&num);
            if(ref!=2104774) break;
            reu_for_one_ref->push_back(new reuse_data(reuse,num));
            sum += num;
        }

        reu_for_all_ref.push_back(reu_for_one_ref);

        //the code below is for printing data which is used by mathematica to draw a 3D pic
        /*pos = 0;
        for(int j=0,size=reu_for_one_ref->size();j<size;j++)
        {
            reuse_data * temp = (*reu_for_one_ref)[j];
            cout << i << "\t" << temp->reuse << "\t" << pos << endl;
            pos += temp->num/(long double)sum;
            cout << i << "\t" << temp->reuse << "\t" << pos << endl;
        }*/

        fclose(in);
    }

    /*for(int i=0,e=reu_for_all_ref.size();i<e;i++)
    {
        reu_for_one_ref = reu_for_all_ref[i];
        for(int j=0,ej=reu_for_one_ref->size();j<ej;j++)
        {
            reuse_data * temp = (*reu_for_one_ref)[j];
            cout << i+10 << "\t" << temp->reuse << "\t" << temp->num << endl;
        }
    }*/
}
