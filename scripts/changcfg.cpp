#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

int main()
{
    ifstream ifs("cgpop.ll");
    ofstream ofs("change.ll");
    char s[1000],bbid[400]={"  br label %"};
    string temp;
    ifs.getline(s,999);
    while(!ifs.eof())
    {
        cout << "#####" << s << "#####" << endl;
        int i = 0;
        while(s[i]==' ' || s[i]=='\t')
        {
            ++i;
        }
        if(s[i]=='b' && s[i+1]=='r')
        {
            ifs.getline(s,999);
            ifs.getline(s,999);
            i=0;
            
            while(s[i]!=':')
            {
                bbid[i+12] = s[i];
                ++i;
            }
            bbid[i+12]='\0';
            ofs << bbid << "\n" << endl;
            ofs << s << endl;
        }
        else ofs << s << endl;
        ifs.getline(s,999);
    }
    ifs.close();
    ofs.close();
    return 0;
}
