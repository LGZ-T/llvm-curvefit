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
}reuse_data;

class module
{
private: 
    ulong ave_reuse, sum_num;
    reuse_data *start, *end, *first;
    module *left, *right, *up, *down;

public:
    module():ave_reuse(0),sum_num(0),start(NULL),end(NULL),
             first(NULL),left(NULL),right(NULL),up(NULL),down(NULL){}

    ulong get_leading_bin() { return start->reuse; }

    void append(reuse_data * ele)
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

    void change_start()
    {
        first = start;
        start = start->next;
    }

    double get_ratio()
    {
        double midpoint = (end->reuse - start->reuse)/2.0;
         
    }
    module * split(double);
};

bool check_leading_bin(module *root)
{
    module *temp = root->down;
    ulong reu = temp->get_leading_bin();
    while(temp!=NULL)
    { 
        if(reu!=temp->get_leading_bin()) return false;
        temp = temp->down;
    }

    return true;
}

void chang_start(module *root)
{
    module *temp = root->down;
    while(temp!=NULL)
    {
        temp->change_start();
        temp = temp->down;
    }
}

void split(module *left, module *right)
{
    if(left is similar with right) return;

    
}

int main()
{
    char filename[50];
    FILE *in;
    unsigned long ref = 2104774, reuse, num, sum;
    long double pos;
    module *root = new module();
    module *pre = root, *cur = NULL;

    for(int i=10;i<=30;i++)//there are 30 files
    {
        sum = 0;
        ref = 2104774;
        sprintf(filename,"mrd10.10.%d",i);
        in = fopen(filename,"r");
        if(in==NULL) printf("it is null\n");
        
        cur = new module();
        pre->down = cur;

        while(1)
        {
            fscanf(in,"%*10c%lu%*7c%lu%*5c%lu%*c",&ref,&reuse,&num);
            if(ref!=2104774) break;
            sum += num;
            cur->append(new reuse_data(reuse,num,sum));
        }
        
        pre = cur;
        fclose(in);
    }
    
    if(check_leading_bin(root))
        change_start(root);
    
    split(root);
}
