/*
 * the structure is
 * PROBLEMSIZE MODULE
 * O------O-------O
 * |      |       |
 * O------O-------O
 * .      .       .
 * .      .       .
 *
 */
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>

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
        next = NULL;
    }
}ReuseData;

class ReuseContainer
{
private: 
    ulong ave_reuse, diff;
    ReuseData *start, *end, *first;
    ReuseContainer *left, *right, *up, *down;

public:
    ReuseContainer():
           ave_reuse(0),start(NULL),end(NULL),first(NULL),
           left(NULL),right(NULL),up(NULL),down(NULL),diff(0) {}

    ReuseContainer(ReuseData *s,ReuseData *e,ulong d):
           ave_reuse(0),start(s),end(e),first(NULL),left(NULL),
           right(NULL),up(NULL),down(NULL),diff(d) {}

    //done
    ulong get_leading_bin() { return start->reuse; }

    //done
    void setStart(ReuseData * s) { start = s; }
    void setEnd(ReuseData *e) { end = e; }
    void setAround(ReuseContainer *l,ReuseContainer *r,ReuseContainer *u,ReuseContainer *d)
    {
        left = l;
        right = r;
        up = u;
        down = d;
    }

    //done
    void append(ReuseData * ele)
    {
        if(start==NULL)
        {
            end = start = ele;
        }
        else
        {
            end->next = ele;
            end = end->next;
        }
    }

    //done
    void change_start()
    {
        first = start;
        start = start->next;
    }
    
    //done
    double get_ratio()
    {
        double midpoint = (end->reuse - start->reuse)/2.0;
        double lessnum = 0;
        double sumnum = first==NULL?end->sum_num:end->sum_num-first->num;

        ReuseData * temp = start;
        while(temp!=NULL)
        {
            if(temp->reuse-midpoint < 0.01 && temp->reuse-midpoint > -0.01)//they are equal
            {
                lessnum += temp->num/2.0;
                break;
            }
            else if(temp->reuse < midpoint) lessnum += temp->num;
            else if(temp->reuse > midpoint) break;
            
            temp = temp->next;
        }

        return lessnum/(sumnum-lessnum);
    }
    
    //wait
    ReuseContainer * split(double ratio)
    {
        diff += (first==NULL?0:first->num);
        ulong leftnum = (1/(1/ratio+1))*(end->sum_num-diff);
        ReuseData *ite = start, *preite = start;
        ReuseContainer * rReuseContainer = NULL;

        while(ite!=NULL)
        {
            if((ite->sum_num-diff)>leftnum)
            {
                ulong curleft = ite->num-(ite->sum_num-diff-leftnum);
                preite->next = new ReuseData(ite->reuse,curleft,leftnum+diff);
                ite->num -= curleft;
                ite->sum_num = ite->num;
                
                rReuseContainer = new ReuseContainer(ite,end,leftnum+diff);
                end = preite->next;
                break;
            }
            else if((ite->sum_num-diff)==leftnum)
            {
                rReuseContainer = new ReuseContainer(ite->next,end,ite->sum_num);
                ite->next = NULL;
                end = ite;
                break;
            }

            preite = ite;
            ite = ite->next;
        }
        return ReuseContainer;
    }
};

//done
bool check_leading_bin(ReuseContainer *root)
{
    ReuseContainer *temp = root->down;
    ulong reu = temp->get_leading_bin();
    while(temp!=NULL)
    { 
        if(reu!=temp->get_leading_bin()) return false;
        temp = temp->down;
    }

    return true;
}

//done
void chang_start(ReuseContainer *root)
{
    ReuseContainer *temp = root->down;
    while(temp!=NULL)
    {
        temp->change_start();
        temp = temp->down;
    }
}

bool comp(double a, double b)
{
    return a < b;
}

//wait
void split(ReuseContainer *root)
{
    int pos = 0, size;
    double finalration;
    vector<double> allratio;
    ReuseContainer *temp = root;
    while(temp!=NULL)
    {
        allratio.push_back(temp->get_ratio());
        temp = temp->down;
    }
    
    sort(allratio.begin(),allratio.end(),comp);
    size = allratio.size();
    finalratio = (allratio[size/2]+allratio[(size-1)/2])/2;
        
}

//wait
int main()
{
    char filename[50];
    FILE *in;
    unsigned long ref = 2104774, reuse, num, sum;
    long double pos;
    ReuseContainer *root = new ReuseContainer();
    ReuseContainer *pre = root, *cur = NULL;

    for(int i=10;i<=30;i++)//there are 30 files
    {
        sum = 0;
        ref = 2104774;
        sprintf(filename,"mrd10.10.%d",i);
        in = fopen(filename,"r");
        if(in==NULL) printf("it is null\n");
        
        cur = new ReuseContainer();
        pre->down = cur;

        while(1)
        {
            fscanf(in,"%*10c%lu%*7c%lu%*5c%lu%*c",&ref,&reuse,&num);
            if(ref!=2104774) break;
            sum += num;
            cur->append(new ReuseData(reuse,num,sum));
        }
        
        pre = cur;
        fclose(in);
    }
    
    if(check_leading_bin(root))
        change_start(root);
    
    split(root);
}
