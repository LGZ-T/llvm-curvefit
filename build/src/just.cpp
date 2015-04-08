#include <iostream>
#include <string>
#include <stdio.h>

using namespace std;

int main()
{
    int num;
    string s("hello:1");
    char cstr[50];
    sscanf(s.c_str(),"%*6c%d",&num);
    printf("%d\n",num);
    return 0;
}
