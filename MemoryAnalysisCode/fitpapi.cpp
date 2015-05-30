#include <iostream>
#include <vector>
#include <stdio.h>
#include <fstream>

using namespace std;

extern void fit(vector<unsigned> *x, long double *y_data, int num, vector<long double> &result);
int main()
{
    vector<unsigned> x_data = {10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30};
    vector<long double> fitresu(12);
    long double hitl1[21], hitl2[21];
    char nouse[13];
    int i = 0;
    ifstream in;
    in.open("cachehitinfo");
    
    for(i=0;i<=20;i++)
    {
        in.get(nouse,13);
        in >> hitl1[i];
        in.get();
        in.get(nouse,13);
        in >> hitl2[i];
        in.get();
    }
    in.close();
    fit(&x_data,hitl1,21,fitresu);
    cout << "hitl1:" << fitresu[0] << "\tnum:" << fitresu[1];
    for(i=0;i<fitresu[1];i++)
    {
        cout << "\t" << fitresu[i+2];
    }
    cout << endl;
    fit(&x_data,hitl2,21,fitresu);
    cout << "fitl2:" << fitresu[0] << "\tnum:" << fitresu[1];
    for(i=0;i<fitresu[1];i++)
    {
        cout << "\t" << fitresu[i+2];
    }
    cout << endl;
    /*for(i=0;i<=20;i++)
    {
        cout << setiosflags(ios::fixed) << "PAPI_L1_TCH:" << hitl1[i] << endl;
        cout << setiosflags(ios::fixed) << "PAPI_L2_TCH:" << hitl2[i] << endl;
    }*/
    

    return 0;
}
