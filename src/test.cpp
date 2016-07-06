#include <iostream>
#include <list>
#include <fstream>
#include <set>
#include <map>

using namespace std;

enum insttype { bbname, getbbtime1, getbbtime2, 
                regu_no_assign, regu_with_assign, 
                outcall, brinst, call_with_assign,
                define, declare, phi};

map<string,string> rettype;
set<string> inhand;

insttype getinsttype(string &str)
{
    if((str[0]=='e' && str[1]=='n' && str[2]=='t' && str[3]=='r') ||
       (str[0]=='r' && str[1]=='e' && str[2]=='t' && str[3]=='u') ||
        str[0]=='"')
    {
        return bbname;
    }
    else if(str.find("phi ")!=string::npos)                                 return phi;
    else if(str[0]=='d' && str[1]=='e' && str[2]=='f' && str[3]=='i')       return define;
    else if(str[0]=='d' && str[1]=='e' && str[2]=='c' && str[4]=='l')       return declare;
    else if(str.compare(0,23,"  call void @getBBTime1")==0)                 return getbbtime1;
    else if(str.compare(0,23,"  call void @getBBTime2")==0)                 return getbbtime2;
    else if(str.find("call")!=string::npos && str.find("=")!=string::npos)  return call_with_assign;
    else if(str.find("call")!=string::npos)                                 return outcall;
    else if(str.find("=")!=string::npos)                                    return regu_with_assign;
    else if(str.find("  br ")!=string::npos)                                return brinst;
    else                                                                    return regu_no_assign;
}

int flag[3000];
int main()
{
    char line[1000];
    list<string> insts;
    ifstream ifs("cgpop.bbtime.ll");
    ofstream ofs("testbench.ll");
    
    ifs.getline(line,1000);
    while(!ifs.eof())
    {
        string linestr(line);
        insttype type = getinsttype(linestr);
        if(type==define || type==declare)
        {
            string temp;
            string tname(line);
            char name[100];
            int i = 6;
            while(line[i]!='@')
            {
                if(line[i]=='v' && line[i+1]=='o' && line[i+2]=='i' && line[i+3]=='d')
                    temp = "void";
                else if(line[i]=='i' && line[i+1]=='3' && line[i+2]=='2')
                    temp = "i32";
                else if(line[i]=='f' && line[i+1]=='l' && line[i+2]=='o')
                    temp = "float";
                else if(line[i]=='d' && line[i+1]=='o' && line[i+2]=='u')
                    temp = "double";
                else if(line[i]=='i' && line[i+1]=='6' && line[i+2]=='4')
                    temp = "i64";
                else temp = "unhandle";
                ++i;
            }
            ++i;
            unsigned int pos = tname.find("(");
            string funcname = tname.substr(i,pos-i);
            rettype.insert(pair<string,string>(funcname,temp));
        }
        ifs.getline(line,1000);
    }

    ifs.close();
    ifs.open("cgpop.bbtime.ll");

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
        }
        else 
        { 
            ofs << line << endl; 
            ifs.getline(line,1000);
            continue; 
        }
       list<string>::iterator ite = insts.begin();
       list<string>::iterator end = insts.end();
       bool print = true;
       list<string>::iterator first, last, brpos;
       while(ite!=end)
       {
           insttype type = getinsttype(*ite);
           if(type==getbbtime1)
           {
               print = true;
               first = ite;
           }
           else if(type==getbbtime2)
           {
               if(print==false)
               {
                   insts.erase(first);
                   ite = insts.erase(ite);
                   continue;
               }
           }
           else if(type==phi)
           {
               string &temp = *ite;
               unsigned int pos, epos;
               pos = temp.find("=");
               string prestr = temp.substr(0,pos);
               pos = temp.find("phi");
               pos += 4;
               epos = temp.find(" ",pos);
               string substr = temp.substr(pos,epos-pos);
               if(substr=="i32")            *ite = prestr.append(" add i32 10, 10");
               else if(substr=="double")    *ite = prestr.append(" add double 10.0, 10.0");
               else if(substr=="float")     *ite = prestr.append(" add float 10.0, 10.0");
               else if(substr=="i64")       *ite = prestr.append(" add i64 10, 10");
           }
           else if(type==outcall) 
           {
               print = false;
               string &temp = *ite;
               if(temp.find("@free")==string::npos)
               {
                   ite = insts.erase(ite);
                   continue;
               }
           }
           else if(type==call_with_assign)
           {
               print = false;
               string &temp = *ite;
               string  tempinst;
               size_t spos = temp.find("@"), epos;
               if(spos==string::npos) cout << "wrong for the call inst" << endl;
               ++spos;
               epos = temp.find(" ",spos);
               string funcname = temp.substr(spos,epos-spos);
               map<string,string>::iterator it = rettype.find(funcname);
               if(it==rettype.end()) cout << "wrong for the map" << endl;
               spos = temp.find("=");
               tempinst = temp.substr(0,spos);
               if(it->second=="i32")           { tempinst.append(" add i32 10, 10"); *ite = tempinst; }
               else if(it->second=="float")    { tempinst.append(" add float 10.0, 10.0"); *ite = tempinst; }
               else if(it->second=="double")   { tempinst.append(" add double 10.0, 10.0"); *ite = tempinst; }
               else if(it->second=="i64")      { tempinst.append(" add i64 10, 10"); *ite = tempinst; }
               else if(it->second=="void")     cout << "it's not possible" << endl;
               else if(it->second=="unhandle") ;
               else                            cout << "wrong" << endl;
           }
           else if(type==brinst)
           {
               //the bb name will be outputed by the next bbname
               *ite = "br label %";
               brpos = ite;
           }
           else if(type==bbname)
           {
               string &temp = *ite;
               unsigned int pos = temp.find(":");
               if(temp.substr(0,pos)=="entry") ;
               else (*brpos).append(temp.substr(0,pos));
           }
           else
           {
               cout << "wrong inst" << endl;
           }
           ++ite;
       }
       ite = insts.begin();
       end = insts.end();
       for(;ite!=end;++ite)
       {
           ofs << *ite << endl;
       }
       insts.clear();
       ofs << "}" << endl << endl;
       ifs.getline(line,1000);
    }
    ifs.close();
    ofs.close();
}


