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
#include <iomanip>
#include <sstream>

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

public:
    ReuseData *first, *start, *end;
    ReuseContainer *down, *right;
    ReuseContainer():
           ave_reuse(0),diff(0),first(NULL),
           start(NULL),end(NULL),down(NULL),right(NULL) {}

    ReuseContainer(ReuseData *s,ReuseData *e,ulong d):
           ave_reuse(0),diff(d),first(NULL),
           start(s),end(e),down(NULL),right(NULL){}

    ulong get_leading_bin() { return start->reuse; }

    //wait, ave_reuse doesnt need to be a func, every time when i change the
    //ReuseContainer, i will change the ave_reuse, and, this func will be called only once.
    //what i should know is when to change ave_reuse,that means, which func can change
    //ave_reuse.this func will be changed later.
    double get_ave_reuse()
    {
        double allnum = end->sum_num - diff, ave=0;
        ReuseData *temp = start;
        while(temp!=NULL)
        {
            ave += (temp->num/allnum)*temp->reuse;
            temp = temp->next;
        }
        ave_reuse = ave;
        return ave;
    }

    //done. append is only be used when reading files
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

    //done this is only be used when all of the leading bins are the same
    //this func should change diff, is it right?
    void change_start()
    {
        first = start;
        start = start->next;
        diff += first->num;
    }
    
    //done
    double get_ratio()
    {
        double midpoint = (end->reuse + start->reuse)/2.0;
        double lessnum = 0;
        double sumnum = end->sum_num-diff;
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
    
    //i think it is done, but wait
    ReuseContainer * split(double ratio)
    {
        ulong leftnum = (ratio/(ratio+1))*(end->sum_num-diff);
        ReuseData *ite = start, *preite = start;
        ReuseContainer * rReuseContainer = NULL;
        
        while(ite!=NULL)
        {
            if((ite->sum_num-diff)>leftnum)
            {
                ulong curleft = ite->num-(ite->sum_num-diff-leftnum);
                preite->next = new ReuseData(ite->reuse,curleft,leftnum+diff);
                ite->num -= curleft;
                
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
        return rReuseContainer;
    }
};

//?????????????????????????????????????????
void print_split(ReuseContainer *header)
{
    ReuseContainer * temp = header->down;
    int i = 10;
    long double pos = 0, sum = 0, diff;

    if(temp->first == NULL) diff = 0;
    else diff = temp->first->num;
    cout << "################################################################" << endl;
    cout << "################################################################" << endl;
    cout << "################################################################" << endl;
    while(temp!=NULL)
    {
        ReuseContainer *tempj = temp;
        while(tempj!=NULL) { sum = tempj->end->sum_num - diff; tempj = tempj->right; }
        
        pos = 0;
        tempj = temp;
        while(tempj!=NULL)
        {
            cout << i << "\t" << tempj->get_ave_reuse() << "\t" << pos << endl;
            pos += (tempj->end->sum_num-tempj->start->sum_num+tempj->start->num)/sum;
            cout << i << "\t" << tempj->get_ave_reuse() << "\t" << pos << endl;
            tempj = tempj->right;
        }
        temp = temp->down;//if i want to add another reference at the end,this should be changed
        i++;
    }
    cout << "################################################################" << endl;
    cout << "################################################################" << endl;
    cout << "################################################################" << endl;
}

bool check_leading_bin(ReuseContainer *root)
{
    ReuseContainer *temp = root->down;
    if(temp==NULL) printf("it is empty\n");
    ulong reu = temp->get_leading_bin();
    while(temp!=NULL)
    { 
        if(reu!=temp->get_leading_bin()) return false;
        temp = temp->down;
    }

    return true;
}

void change_start(ReuseContainer *root)
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

extern void fit(vector<unsigned> *x, long double *y_data, int num,vector<long double> &result);
static vector<long double> fitresu(12);//10+2 should be replaced with PARA_MAX,i don't know how to do
static vector<unsigned> x_data = {10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25};
static long double PData[16], RData[16];
static double totalnum[21];
static FILE *out;

void constructPandR(ReuseContainer *left, ReuseContainer *right)
{
    ReuseContainer * templ = left, *tempr = right;

    int i = 0;
    while(i<=15)
    {
        PData[i] = (tempr->start->sum_num - tempr->start->num)/totalnum[i];
        RData[i] = templ->get_ave_reuse();
        tempr = tempr->down;
        templ = templ->down;
        i++;
    }
}

void print_func_paras(vector<long double> &fitresu, long double *data)
{
    int paranum = fitresu[1];
    for(int i=0;i<16;i++)
    {
        fprintf(out,"%Lf ",*(data+i));
    }
    fprintf(out,"\n");
    for(int i=1;i<=paranum+1;i++)
    {
        fprintf(out,"%Lf\t",fitresu[i]);
        //cout << fitresu[i] << "\t";
    }
    fprintf(out,"\n");
}

//wait
/*how to judje if two modules are similiar
 * the two modules have teh similiar fitting curve
 */
void split(ReuseContainer *root, ReuseContainer *header)
{
    bool isContinue = true,lContinue = true,rContinue = true;
    vector<double> allratio;
    ReuseContainer *temp = root, *right=NULL,*splitup=NULL;
    while(temp!=NULL)
    {
        allratio.push_back(temp->get_ratio());
        temp = temp->down;
    }
    
    sort(allratio.begin(),allratio.end(),comp);
    int size = allratio.size();
    double finalratio = (allratio[size>>1]+allratio[(size-1)>>1])/2;

    if(finalratio<0.0001 && finalratio>-0.0001) return;//finalratio will never be 0,this is not gonna to happen

    temp = root;
    while(temp!=NULL)
    {
        right = temp->split(finalratio);
        if(splitup!=NULL) splitup->down = right;
        right->right = temp->right;
        temp->right = right;
        splitup = right;
        long diffreuse = temp->get_ave_reuse()-right->get_ave_reuse();
        if(diffreuse < 2 && diffreuse > -2) isContinue = false;
        if(temp->start==temp->end)            lContinue  = false;
        if(right->start==right->end)          rContinue  = false;
        temp = temp->down;
    }
    right = root->right;
    constructPandR(root,right);
    fit(&x_data,PData,16,fitresu);
    print_func_paras(fitresu,PData);
    fit(&x_data,RData,16,fitresu);
    print_func_paras(fitresu,RData);
    print_split(header);
    if(isContinue==false) return;
    if(lContinue==false && rContinue==false) return;
    if(lContinue==false) split(right,header);
    else if(rContinue==false) split(root,header);
    else { split(root,header); split(right,header); }
}

void print_data(ReuseContainer *root)
{
    ReuseContainer *temp = root->down, *tempj;
    int i=10;
    while(temp!=NULL)
    {
        cout << i << "\t";
        if(temp->first!=NULL)
        {
            cout <<  "[" << temp->first->reuse << ":"
                 << temp->first->num << "]" << "###";
        }
        tempj = temp;
        while(tempj!=NULL)
        {
            cout << "[" << tempj->get_ave_reuse() << ":" 
                 << tempj->end->sum_num-tempj->start->sum_num + tempj->start->num << "]\t";
            tempj = tempj->right;
        }
        cout << endl;
        temp = temp->down;
        i++;
    }
}

void print_origin(ReuseContainer *root)
{
    ReuseContainer *temp = root->down;
    ReuseData *tempj = NULL;
    int i = 10;
    while(temp!=NULL)
    {
        cout << i << "\t";
        tempj = temp->start;
        while(tempj!=NULL)
        {
            cout << "[" << tempj->reuse << ":" << tempj->num << "] ";
            tempj = tempj->next;
        }
        cout << endl;
        i++;
        temp = temp->down;
    }
}

//wait
int main()
{
    char filename[50];
    FILE *in;
    unsigned long curref, preref = 0, reuse, num, sum, curdsize, predsize = 0;
    ReuseContainer *root = new ReuseContainer();
    ReuseContainer *pre = root, *cur = NULL;
    
    out = fopen("fitFuncParas","w");
    in = fopen("reusedata","w");
    fscanf(in,"%*9c%lu%*10c%lu%*7c%lu%*5c%lu%*c",&datasize,&curref,&reuse,&num);
    while(!feof(in))
    {
        if(curref)
        cur = new ReuseContainer();
        pre->down = cur;
        
    }
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
        totalnum[i-10] = cur->end->sum_num;
        pre = cur;
        fclose(in);
    }
    print_origin(root);
    bool result = check_leading_bin(root);
    if(result)
    {
        change_start(root);
    }
    split(root->down,root);
    print_data(root);
    return 0;
}
