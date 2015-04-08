#include <map>
#include <cmath>
#include <stack>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
using namespace std;

typedef unsigned long unlong;
typedef struct bal
{
    unlong counter;//timestamp
    unlong RNum;//num of all children
    int depth;
    struct bal *child[2];//0 lchild,1 rchild
    bal():counter(0),RNum(0),depth(0)
    {
        child[0] = NULL;
        child[1] = NULL;
    }
    bal(unlong c, unlong r, int d, struct bal *c1, struct bal *c2):
    counter(c),RNum(r),depth(d)
    {
        child[0] = c1;
        child[1] = c2;
    }

}BALNODE;
BALNODE root = BALNODE();

map<unlong,map<unlong,unlong> >result;
map<unlong,unlong> refmap;
unlong global_counter = 0;
#define DELNODE(child_) \
        BALNODE* father = fa_stack.top();\
        int r_or_l = rorl_stack.top();\
        fa_stack.pop();\
        rorl_stack.pop();\
        delete node;\
        father->child[r_or_l] = child_;\
        node = father;\
        caldepth(node);\
        calRNum(node);\
        while(!fa_stack.empty())\
        {\
            father = fa_stack.top();\
            r_or_l = rorl_stack.top();\
            fa_stack.pop();\
            rorl_stack.pop();\
            caldepth(father);\
            calRNum(father);\
            adjust(node,father,r_or_l);\
            node = father;\
        }

inline void caldepth(BALNODE* node)
{
    if((node->child[0]==NULL)&&(node->child[1]==NULL)) node->depth = 0;
    else if((node->child[0]==NULL)&&(node->child[1]!=NULL)) node->depth = node->child[1]->depth+1;
    else if((node->child[0]!=NULL)&&(node->child[1]==NULL)) node->depth = node->child[0]->depth+1;
    else node->depth = node->child[0]->depth>node->child[1]->depth?node->child[0]->depth+1:node->child[1]->depth+1;
}

inline void calRNum(BALNODE* node)
{
    if((node->child[0]==NULL)&&(node->child[1]==NULL)) node->RNum = 0;
    else if((node->child[0]==NULL)&&(node->child[1]!=NULL)) node->RNum = node->child[1]->RNum+1;
    else if((node->child[0]!=NULL)&&(node->child[1]==NULL)) node->RNum = node->child[0]->RNum+1;
    else node->RNum = node->child[0]->RNum+node->child[1]->RNum+2;
}

//r_or_l 0 node is the lchild,1 node is the rchild. rotate 0 is LL, 1 is RR
inline void LLorRR(BALNODE* node, BALNODE* father, int r_or_l, int rotate)
{
    BALNODE* node2 = node;
    BALNODE* node1 = node->child[rotate];

    father->child[r_or_l] = node1;
    node2->child[rotate] = node1->child[1-rotate];
    node1->child[1-rotate] = node2;

    caldepth(node2);
    if(node1->child[rotate]==NULL)
    {
        node1->depth = node2->depth+1;
        node1->RNum = node2->RNum;
        node2->RNum--;
    }
    else
    {
        node1->depth = node1->child[rotate]->depth>node2->depth?node1->child[rotate]->depth+1:node2->depth+1;
        node1->RNum = node2->RNum;
        node2->RNum = node2->RNum-1-(node1->child[rotate]->RNum+1);
    }
}

//rotate 0 uses LL, 1 uses RR
inline void RLorLR(BALNODE* node, BALNODE* father, int r_or_l, int rotate)
{
    BALNODE* node3 = node;
    BALNODE* node2 = node->child[1-rotate];
    BALNODE* node1 = node->child[1-rotate]->child[rotate];

    LLorRR(node2,node3,1-rotate,rotate);

    father->child[r_or_l] = node1;
    node3->child[1-rotate] = node1->child[rotate];
    node1->child[rotate] = node3;

    caldepth(node3);
    node1->depth = node2->depth>node3->depth?node2->depth+1:node3->depth+1;
    node1->RNum = node3->RNum;
    node3->RNum = node3->RNum-node2->RNum-2;
}

inline void adjust(BALNODE* node, BALNODE* father, int r_or_l)
{
    int ldepth,rdepth,lldepth,lrdepth,rldepth,rrdepth;
    if(node->child[0]==NULL) {ldepth = lldepth = lrdepth = -1;}
    else
    {
        ldepth = node->child[0]->depth;
        if(node->child[0]->child[0]==NULL) lldepth = -1;
        else lldepth = node->child[0]->child[0]->depth;

        if(node->child[0]->child[1]==NULL) lrdepth = -1;
        else lrdepth = node->child[0]->child[1]->depth;
    }

    if(node->child[1]==NULL) {rdepth = rldepth = rrdepth = -1;}
    else
    {
        rdepth = node->child[1]->depth;
        if(node->child[1]->child[0]==NULL) rldepth = -1;
        else rldepth = node->child[1]->child[0]->depth;

        if(node->child[1]->child[1]==NULL) rrdepth = -1;
        else rrdepth = node->child[1]->child[1]->depth;
    }

    if(abs(rdepth-ldepth)<=1) return;
    if((ldepth>rdepth)&&(lldepth>=lrdepth))         LLorRR(node,father,r_or_l,0);
    else if((rdepth>ldepth)&&(rrdepth>=rldepth))    LLorRR(node,father,r_or_l,1);
    else if((ldepth>rdepth)&&(lrdepth>lldepth))     RLorLR(node,father,r_or_l,1);
    else if((rdepth>ldepth)&&(rldepth>rrdepth))     RLorLR(node,father,r_or_l,0);
    else cout << "wrong in adjust" << endl;
}

void insert(unlong local_count)
{
    stack<BALNODE*> fa_stack;
    stack<int> rorl_stack;
    BALNODE *node = root.child[0];
    int r_or_l = 0;
    fa_stack.push(&root);
    rorl_stack.push(0);
    /*there is no node in the tree*/
    if(node==NULL)
    {
        root.child[0] = new BALNODE(local_count,0,0,NULL,NULL);
        return;
    }

    while(1)
    {
        node->RNum++;
        if(local_count>node->counter)
        {
            if(node->child[1]==NULL)
            {
                node->child[1] = new BALNODE(local_count,0,0,NULL,NULL);
                caldepth(node);
                while(!fa_stack.empty())
                {
                    BALNODE *father = fa_stack.top();
                    fa_stack.pop();
                    int rorl = rorl_stack.top();
                    rorl_stack.pop();
                    caldepth(father);
                    adjust(node,father,rorl);
                    node = father;
                }
                return;
            }
            else
            {
                fa_stack.push(node);
                rorl_stack.push(1);
                node = node->child[1];
            }
        }
        else if(local_count<node->counter)
        {
            if(node->child[0]==NULL)
            {
                node->child[0] = new BALNODE(local_count,0,0,NULL,NULL);
                caldepth(node);
                while(!fa_stack.empty())
                {
                    BALNODE *father = fa_stack.top();
                    fa_stack.pop();
                    int rorl = rorl_stack.top();
                    rorl_stack.pop();
                    caldepth(father);
                    adjust(node,father,rorl);
                    node = father;
                }
                return;
            }
            else
            {
                fa_stack.push(node);
                rorl_stack.push(0);
                node = node->child[0];
            }
        }
        else {cout << "how can it be there" << endl; return;}
    }
}

void delnode(unlong local_count, unlong *reuse)
{
    stack<BALNODE*> fa_stack;
    stack<int> rorl_stack;
    BALNODE* node;
    node =  root.child[0];
    fa_stack.push(&root);
    rorl_stack.push(0);
    /*there is no node in the tree*/
    if(node==NULL) { cout << "you idiot, there is no node in the tree, how can i delete" << endl;  return;}
    while(1)
    { 
        if(local_count>node->counter)
        {
            fa_stack.push(node);
            rorl_stack.push(1);
            node = node->child[1];
        }
        else if(local_count<node->counter)
        {
            if(node->child[1]!=NULL) *reuse += node->child[1]->RNum+2;
            fa_stack.push(node);
            rorl_stack.push(0);
            node = node->child[0];
        }
        else
        {

            if((node->child[0]==NULL)&&(node->child[1]==NULL))
            {
                DELNODE(NULL);
                return;
            }
            else if((node->child[0]==NULL)&&(node->child[1]!=NULL))
            {
                *reuse += node->child[1]->RNum+1;
                BALNODE* child = node->child[1];
                DELNODE(child);
                return;
            }
            else if((node->child[0]!=NULL)&&(node->child[1]==NULL))
            {
                BALNODE* child = node->child[0];
                DELNODE(child);
                return;
            }
            else
            {
                *reuse += node->child[1]->RNum+1;
                BALNODE* child = node->child[0];
                while(child->child[1]!=NULL)
                {
                    child = child->child[1];
                }
                local_count = node->counter = child->counter;
                fa_stack.push(node);
                rorl_stack.push(0);
                node = node->child[0];
            }
        }
    }
    
}


extern "C" {
    void getReuseDistance(unsigned int value);
    //void outinfo();
}

void getReuseDistance(unsigned int value)
{
    //global_counter++;
    unlong ref = ((unlong)value)/64;
    unlong local_counter = global_counter++;
    map<unlong,unlong>::iterator iter = refmap.find(ref);
    if(iter==refmap.end())
    {
        refmap.insert(pair<unlong,unlong>(ref,local_counter));
        insert(local_counter);
    }
    else
    {
        unlong reuse = 0;
        delnode(iter->second,&reuse);
        map<unlong,map<unlong,unlong> >::iterator ite1 = result.find(iter->first);
        if(ite1==result.end())
        {
            map<unlong,unlong> temp;
            temp.insert(pair<unlong,unlong>(reuse,1));
            result.insert(make_pair(iter->first,temp));
        }
        else
        {
            map<unlong,unlong> &reuseinfo = ite1->second;
            map<unlong,unlong>::iterator ite2 = reuseinfo.find(reuse);

            if(ite2==reuseinfo.end())  reuseinfo.insert(pair<unlong,unlong>(reuse,1));
            else ite2->second++;
        }
        iter->second = local_counter;
        insert(local_counter);
    }
}

void outinfo()
{
    //printf("%lu\n",global_counter); 
    unlong pid = (unlong)getpid();
    char cpid[20];
    sprintf(cpid,"mrd1%lu",pid);
    ofstream out(cpid);
    map<unlong,map<unlong,unlong> >::iterator ite1 = result.begin(),end1 = result.end();
    for(;ite1!=end1;ite1++)
    {
        map<unlong,unlong>::iterator ite2 = ite1->second.begin(),end2 = ite1->second.end();
        for(;ite2!=end2;ite2++)
            out<<"reference:"<<ite1->first<<"\t"<<"reuse:"<<ite2->first<<"\t"<<"num:"<<ite2->second<<endl;
    }
    out.close();
}

/*void print_tree(BALNODE *node)
{
    if(node==NULL) return;
    cout << "timestamp:" << node->counter << "##num:" << node->RNum << "##depth:" << node->depth << endl;
    print_tree(node->child[0]);

    print_tree(node->child[1]);
}*/
