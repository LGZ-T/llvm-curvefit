#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

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

//vector<vector<reuse_data *> *> result;

void process_data(vector<vector<reuse_data*> *> &alldata)
{
    //first check the leading bins of each problem size
    vector<reuse_data*> *temp = alldata[0];
    unsigned long equal = (*temp)[0]->reuse;  
    int start = 0;
    for(int s=0,e=alldata.size();s<e;s++)
        if((*alldata[s])[0]->reuse!=equal) break;

    if(s==e) start = 1;//all of the leading bins are equal
    

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