#include <iostream>
#include <list>
#include <fstream>

using namespace std;

enum insttype {bbname, getbbtime1, getbbtime2, regu_no_assign, regu_with_assign, outcall, brinst};

insttype getinsttype(string &str)
{
    if((str[0]=='e' && str[1]=='n' && str[2]=='t' && str[3]=='r') ||
       (str[0]=='r' && str[1]=='e' && str[2]=='t' && str[3]=='u') ||
        str[0]=='"')
    {
        return bbname;
    }
    else if()
}

int main()
{
    char line[1000];
    list<string> insts;
    ifstream ifs("cgpop.bbtime.ll");
    ofstream ofs("testbeck.ll");
    
    ifs.getline(line,1000);
    while(!ifs.eof())
    {
        if(line[0]=='d' && line[1]=='e' && line[2]=='f' && line[3]=='i')
        {
            ofs << line << endl;
            ifs.getline(line,1000);

            //read the whole func into list
            while(line[0]!='}')
            {
                insts.push_back(string(line));
                ifs.getline(line,1000);
            }
            list<string>::iterator ite = insts.begin();
            list<string>::iterator end = insts.end();
            while(ite!=end)
            {
                insttype type = getinsttype(*ite)
            }
        }
    }
}


