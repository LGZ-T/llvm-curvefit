#include <iostream>
#include <map>
#include <cmath>

using namespace std;

typedef unsigned long unlong;
typedef struct bal
{
    unlong counter;//timestamp
    unlong RNum;//num of all children
    int depth;
    struct bal *child[2];//0 lchild,1 rchild
    //struct bal *father;
}BALNODE;
BALNODE root = {0};

map<unlong,unlong> refmap;
unlong global_counter = 0;

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

void insert(unlong local_count, BALNODE* node, BALNODE* father, int r_or_l)
{
    /*there is no node in the tree*/
    if(node==NULL)
    {
        //root.child[0] = new BALNODE(local_count,0,0,{NULL,NULL},&root);
        root.child[0] = new BALNODE();
        root.child[0]->counter = local_count;
        root.child[0]->child[0] = NULL;
        root.child[0]->child[1] = NULL;
        root.child[0]->RNum = 0;
        root.child[0]->depth = 0;
        return;
    }
    node->RNum++;
    if(local_count>node->counter)
    {
        if(node->child[1]==NULL)
        {
            //node->child[1] = new BALNODE(local_count,0,0,{NULL,NULL},node);
            node->child[1] = new BALNODE();
            node->child[1]->counter = local_count;
            node->child[1]->child[0] = NULL;
            node->child[1]->child[1] = NULL;
            node->child[1]->RNum = 0;
            node->child[1]->depth = 0;
            caldepth(node);
            return;
        }
        else
        {
            insert(local_count,node->child[1],node,1);
            caldepth(node);
            adjust(node,father,r_or_l);
            return;
        }
    }
    else if(local_count<node->counter)
    {
        if(node->child[0]==NULL)
        {
            //node->child[0] = new BALNODE(local_count,0,0,{NULL,NULL},node);
            node->child[0] = new BALNODE();
            node->child[0]->counter = local_count;
            node->child[0]->child[0] = NULL;
            node->child[0]->child[1] = NULL;
            node->child[0]->RNum = 0;
            node->child[0]->depth = 0;
            caldepth(node);
            return;
        }
        else
        {
            insert(local_count,node->child[0],node,0);
            caldepth(node);
            adjust(node,father,r_or_l);
            return;
        }
    }
}

void delnode(unlong local_count, BALNODE* node, BALNODE* father, int r_or_l, unlong *reuse)
{
    /*there is no node in the tree*/
    if(node==NULL) { cout << "you idiot, there is no node in the tree, how can i delete" << endl;  return;}
    if(local_count>node->counter)
    {
        if(node->child[1]==NULL)
        {
            cout << "you idiot, there is a bug in your program, how can it be the null" << endl;
            return;
        }
        else
        {
            delnode(local_count,node->child[1],node,1,reuse);
            caldepth(node);
            calRNum(node);
            adjust(node,father,r_or_l);
            return;
        }
    }
    else if(local_count<node->counter)
    {
        if(node->child[1]!=NULL) *reuse += node->child[1]->RNum+2;
        if(node->child[0]==NULL)
        {
            cout << "you idiot, there is a bug in your program, how can it be the null" << endl;
            return;
        }
        else
        {
            delnode(local_count,node->child[0],node,0,reuse);
            caldepth(node);
            calRNum(node);
            adjust(node,father,r_or_l);
            return;
            //there is something else to do....
        }
    }
    else
    {
        if((node->child[0]==NULL)&&(node->child[1]==NULL))
        {
            //BALNODE* father = node->father;
            delete node;
            father->child[r_or_l] = NULL;
            return;
        }
        else if((node->child[0]==NULL)&&(node->child[1]!=NULL))
        {
            //BALNODE* father = node->father;
            *reuse += node->child[1]->RNum+1;
            BALNODE* child = node->child[1];
            delete node;
            father->child[r_or_l] = child;
            return;
        }
        else if((node->child[0]!=NULL)&&(node->child[1]==NULL))
        {
            //BALNODE* father = node->father;
            BALNODE* child = node->child[0];
            delete node;
            father->child[r_or_l] = child;
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
            node->counter = child->counter;
            delnode(node->counter,node->child[0],node,0,reuse);

            caldepth(node);
            calRNum(node);
            adjust(node,father,r_or_l);
            return;
        }
    }
}


extern "C" {
    void getReuseDistance(unsigned int value);
    void outinfo();
}

void getReuseDistance(unsigned int value)
{
    unlong ref = (unlong)value;
    unlong local_counter = global_counter++;
    cout << global_counter << endl;
    map<unlong,unlong>::iterator iter = refmap.find(ref);
    if(iter==refmap.end())
    {
        refmap.insert(pair<unlong,unlong>(ref,local_counter));
        insert(local_counter,root.child[0],&root,0);
    }
    else
    {
        unlong reuse = 0;
        delnode(iter->second,root.child[0],&root,0,&reuse);
        cout << "ref:" << hex << iter->first  << "\treuse:" << reuse << endl;
        iter->second = local_counter;
        insert(local_counter,root.child[0],&root,0);
    }

}

/*void print_tree(BALNODE *node)
{
    if(node==NULL) return;
    cout << "timestamp:" << node->counter << "##num:" << node->RNum << "##depth:" << node->depth << endl;
    print_tree(node->child[0]);

    print_tree(node->child[1]);
    getReuseDistance(1);
}*/
