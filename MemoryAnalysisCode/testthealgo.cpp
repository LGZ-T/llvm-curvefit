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
class ReuseContainer;
void print_split(ReuseContainer *left, ReuseContainer *right);
class ReuseContainer
{
private: 
    ulong ave_reuse, diff;//when should diff be changed????

public:
    ReuseData *first, *start, *end;
    ReuseContainer *down, *right;
    ReuseContainer():
           ave_reuse(0),diff(0),first(NULL),
           start(NULL),end(NULL),down(NULL),right(NULL) {}

    ReuseContainer(ReuseData *s,ReuseData *e,ulong d):
           ave_reuse(0),diff(d),first(NULL),
           start(s),end(e),down(NULL),right(NULL){}

    //done
    ulong get_leading_bin() { return start->reuse; }

    //done
    /*void setStart(ReuseData * s) { start = s; }
    void setEnd(ReuseData *e) { end = e; }
    void setAround(ReuseContainer *l,ReuseContainer *r,ReuseContainer *u,ReuseContainer *d)
    {
        left = l;
        right = r;
        up = u;
        down = d;
    }*/

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
        //return ave_reuse;
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
        //cout << "when get ration: max reuse is:" << end->reuse << "##" << "min reuse is:" 
         //    << start->reuse << "##midpoint is:" << midpoint << "##sumnum is:" << sumnum << "##";
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
        //diff += (first==NULL?0:first->num);
        //cout << "ration is = " << ratio << endl;
        ulong leftnum = (1/(1/ratio+1))*(end->sum_num-diff);
        ReuseData *ite = start, *preite = start;
        ReuseContainer * rReuseContainer = NULL;
        //ReuseData *temp = start;
        /*cout << "before split ###";
        while(temp!=NULL)
        {
            cout << "[" << temp->reuse << ":" << temp->num << "]\t";
            temp = temp->next;
        }
        cout << endl;*/
        while(ite!=NULL)
        {
            if((ite->sum_num-diff)>leftnum)
            {
                //cout << "ite->sum_num is:" << ite->sum_num << "\tdiff is:"<< diff << "\tite->num is:"<<ite->num 
                 //    << "\tleftnum is:" << leftnum << "\treuse is:" << ite->reuse << endl;
                ulong curleft = ite->num-(ite->sum_num-diff-leftnum);
                preite->next = new ReuseData(ite->reuse,curleft,leftnum+diff);
                ite->num -= curleft;
                //ite->sum_num = ite->num;ite->sum_num do not need to be changed
                
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
        //cout << "after split ###";
        //print_split(this,rReuseContainer);
        //cout << endl;
        return rReuseContainer;
    }
};

//done
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

//done
void change_start(ReuseContainer *root)
{
    ReuseContainer *temp = root->down;
    printf("i was in change_start\n");
    while(temp!=NULL)
    {
        temp->change_start();
        temp = temp->down;
    }
    printf("i was after change_start\n");
}

bool comp(double a, double b)
{
    return a < b;
}

void print_split(ReuseContainer *left, ReuseContainer *right, int i)
{   
    ReuseData *temp = left->start;
    cout << i << "\t";
    while(temp!=NULL)
    {
        cout << "[" << temp->reuse << ":" << temp->num << "]\t";
        temp = temp->next;
    }
    temp = right->start;
    cout << "####";
    while(temp!=NULL)
    {
        cout << "[" << temp->reuse << ":" << temp->num << "]\t";
        temp = temp->next;
    }
    cout << endl;
}
//wait
/*how to judje if two modules are similiar
 * 1. if the module has only one reuse distance
 * 2. if the averge reuse distances of two modules have one(two,three,.....i don't know) difference
 */
void split(ReuseContainer *root)
{
    bool isContinue = true,lContinue = true,rContinue = true;
    vector<double> allratio;
    ReuseContainer *temp = root, *right=NULL,*splitup=NULL;
    //cout << "split ratios are: ";
    while(temp!=NULL)
    {
        allratio.push_back(temp->get_ratio());
        //cout << allratio.back() << endl;
        temp = temp->down;
    }
    //cout << endl;
    
    sort(allratio.begin(),allratio.end(),comp);
    int size = allratio.size();
    double finalratio = (allratio[size/2]+allratio[(size-1)/2])/2;
    if(finalratio<0.0001 && finalratio>-0.0001) return;
    temp = root;
    //cout << "final ration is " << finalratio << endl;
    int i = 10;
    while(temp!=NULL)
    {
        right = temp->split(finalratio);
        if(splitup!=NULL) splitup->down = right;
        right->right = temp->right;
        temp->right = right;
        splitup = right;
        print_split(temp,right,i++);
        ulong diffreuse = temp->get_ave_reuse()-right->get_ave_reuse();
        if(diffreuse < 10 && diffreuse > -10) isContinue = false;
        if(temp->start==temp->end)            lContinue  = false;
        if(right->start==right->end)          rContinue  = false;
        
        temp = temp->down;
    }
    cout << "\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\t\n" << endl;
    right = root->right;
    if(isContinue==false) return;

    if(lContinue==false && rContinue==false) return;

    if(lContinue==false) split(right);
    else if(rContinue==false) split(root);
    else { split(root); split(right); }
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
                 << temp->first->num << "]" << "\t";
        }
        tempj = temp;
        while(tempj!=NULL)
        {

            ostringstream ostr;
            ostr << "[" << tempj->get_ave_reuse() << ":" 
                 << tempj->end->sum_num-tempj->start->sum_num + tempj->start->num << "]";
            cout << left;
            cout << setw(13) << ostr.str();
            ostr.clear();
            //cout << "[" << tempj->get_ave_reuse() << ":" 
             //    << tempj->end->sum_num-tempj->start->sum_num + tempj->start->num << "]" << "\t\t";
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
    unsigned long ref = 2104774, reuse, num, sum;
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
    //print_origin(root);
    bool result = check_leading_bin(root);
    if(result)
    {
        change_start(root);
    }
    split(root->down);
    cout << "#################################################################" << endl;
    print_data(root);
}
