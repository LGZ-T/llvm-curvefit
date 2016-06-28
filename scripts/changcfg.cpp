#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>

using namespace std;

int main()
{
    ifstream ifs("cgpop.real.ll");
    ofstream ofs("change.ll");
    char s[1000],bbid[400]={"  br label %"};
    char phii32[100]={"add i32 10, 10"}, phii64[100]={"add i64 10, 10"}, phidouble[100]={"fadd double 10.5, 10.5"};
    char phii1[100]={"add i1 false, true"}, phi0xi32[100]={"bitcast  "};
    char phi[100]={"  "};
    string preinst(500,'\0');
    list<string> restru;
    string temp;
    ifs.getline(s,999);
    while(!ifs.eof())
    {
        //cout << "#####" << s << "#####" << endl;
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
        else if(s[i]=='%')
        {
            int j = 2;
            phi[j++] = s[i++];
            while(s[i]!='=')  phi[j++] = s[i++];
            phi[j] = '\0';
            ++i;
            while(s[i]==' ') ++i;
            if(s[i]=='p' && s[i+1]=='h' && s[i+2]=='i')
            {
                i += 3;
                while(s[i]==' ') ++i;
                if(s[i]=='d' && s[i+1]=='o' && s[i+2]=='u')
                {
                    ofs << phi << "= " << phidouble << endl;
                }
                else if(s[i]=='i' && s[i+1]=='3' && s[i+2]=='2')
                {
                    ofs << phi << "= " << phii32 << endl;
                }
                else if(s[i]=='i' && s[i+1]=='6' && s[i+2]=='4')
                {
                    ofs << phi << "= " << phii64 << endl;
                }
                else if(s[i]=='i' && s[i+1]=='1' && s[i+2]==' ')
                {
                    ofs << phi << "= " << phii1 << endl;
                }
                else
                {
                    cout << "Do not think that will happen" << endl;
                    cout << "###" << s << "###" << endl;
                    ofs << s << endl;
                }
            }
            else ofs << s << endl;
        }
        else ofs << s << endl;
        ifs.getline(s,999);
    }
    ifs.close();
    ofs.close();

    ifs.open("change.ll");
    ofs.open("finalchange.ll");
    int k, h;
    preinst[0] = '\0';
    cout << "find changen\n" << endl;
    ifs.getline(s,999);
    while(!ifs.eof())
    {
        if(s[0]=='"' || (s[0]=='r' && s[1]=='e' && s[2]=='t') || (s[0]=='e' && s[1]=='n' && s[2]=='t'))
        {
            cout << s << endl;
            restru.push_back(string(s));
            while(true)
            {
                ifs.getline(s,999);
                if(s[0]=='\0') break;
                restru.push_back(string(s));
            }
            string &str = restru.back();
            k = 0;
            while(str[k]==' ' || str[k]=='\t') ++k;
            if(str[k]=='u' && str[k+1]=='n' && str[k+2]=='r')
            {

                while(true) 
                {
                    ifs.getline(s,999);
                    if(s[0]!='\0') break;
                }

                k = 0;
                while(s[k]!=':') ++k;
                s[k] = '\0';
                h = 0;
                while(preinst[h]!='%') ++h;
                //preinst[h+1] = '\0';
                ofs << preinst.substr(0,h+1)+string(s) << "\n" << endl;
                s[k] = ':';
                preinst[0] = '\0';
                for(list<string>::iterator it=restru.begin(),end=restru.end();it!=end;++it)
                {
                    ofs << *it << endl;
                }
                ofs << endl;
                restru.clear();
                continue;
            }
            else
            {
                list<string>::iterator end = restru.end();
                --end;
                if(preinst[0]!='\0')
                    ofs << preinst << "\n" << endl;
                for(list<string>::iterator it=restru.begin();it!=end;++it)
                {
                    ofs << *it << endl;
                }
                cout << "restru.back() is:" << *end << endl;
                preinst = *end;
                cout << "preinst is:" << preinst << endl;
                if(preinst[0]=='}') {ofs << preinst << "\n" << endl; preinst[0] = '\0';}
                restru.clear();
                cout << "is this" << endl;
            }
        }
        else
        {
            ofs << s << endl;
        }
       ifs.getline(s,999);
    }
    ifs.close();
    ofs.close();

    return 0;
}
