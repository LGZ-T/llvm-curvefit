#include <iostream>
#include <list>
#include <fstream>
#include <set>

using namespace std;

enum insttype {bbname, getbbtime1, getbbtime2, regu_no_assign, regu_with_assign, outcall, brinst, call_with_assign};

set<string> inhand;
void process_right_operands(string &str)
{
    set<string>::iterator it = inhand.find(str);
    if(it==set::end)
    {

    }
}
insttype getinsttype(string &str)
{
    if((str[0]=='e' && str[1]=='n' && str[2]=='t' && str[3]=='r') ||
       (str[0]=='r' && str[1]=='e' && str[2]=='t' && str[3]=='u') ||
        str[0]=='"')
    {
        return bbname;
    }
    else if(str.compare(0,23,"  call void @getBBTime1")==0)
    {
        return getbbtime1;
    }
    else if(str.compare(0,23,"  call void @getBBTime2")==0)
    {
        return getbbtime2;
    }
    else if(str.find("call")!=string::npos && str.find("=")!=string::npos)
    {
        return call_with_assign;
    }
    else if(str.find("call")!=string::npos)
    {
        return outcall;
    }
    else if(str.find("=")!=string::npos)
    {
        return regu_with_assign;
    }
    else if(str.find("  br ")!=string::npos)
    {
        return brinst;
    }
    else
        return regu_no_assign;
}

int flag[3000];
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
            vector<string> search;
            while(ite!=end)
            {
                ++ite;
                string &str = *ite;
                if(str.find("phi")!=string::npos) //do something special;
                int length = str.length();

                for(int i=0;i<length;++i)
                {
                    if(str[i]=='%' && str[i+1]>='0' && str[i+1]<='9')
                    {
                        ++i;
                        while(str[i]>='0' && str[i]<='9')
                        {
                            
                        }
                    }
                }
            }
        }
        else 
        {
            //do something;
            continue;
        }
        while(ite!=end)
        {
            insttype type = getinsttype(*ite);
            if(type==bbname) 
            {
                //do something,complete the last br label
                ofs << *ite << endl;
            }
            else if(type==getbbtime1)
            {
                ++ite;
                type = getinsttype(*ite);
                while(type!=getbbtime2)
                {
                    if(type==regu_no_assign)
                    {
                        process_right_operands(*ite);
                    }
                    else if(type==regu_with_assign)
                    {
                        update_inhand_list(*ite);
                    }
                    else if(type==outcall || type==call_with_assign)
                    {
                        //do not output this instruction
                    }
                    else if(type==brinst)
                    {
                        //the bb name will be outputed by the next bbname
                        ofs << "br label ";
                    }
                    else if(type==bbname)
                    {
                        string &temp = *ite;
                        unsigned int pos = temp.find(":");
                        if(temp.substr(0,pos)=="entry") ofs << *ite << endl;
                        else ofs << "%" << temp.substr(0,pos) <<  endl << endl << *ite << endl;
                    }
                    else
                    {
                        cout << "wrong inst" << endl;
                    }
                }
            }
        }
    }
}


