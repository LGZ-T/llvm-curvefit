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
    int k, h;
    perinst[0] = '\0';
    ifs.getline(s,999);
    while(!ifs.eof())
    {
        if(s[0]=='"')
        {
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
                restru.clear();
                ifs.getline(s,999);
                ifs.getline(s,999);
                k = 0;
                while(s[k]!=':') ++k;
                s[k] = '\0';
                k = 0;
                while(preinst[k]!='"') ++k;
                h = 0;
                while(s[h]!='\0')
                {
                    preinst[k+h] = s[h];
                    ++h;
                }
                preinst[k+h]='\0';
                ofs << preinst << "\n" << endl;
                s[h] = ':';
                s[h+1] = '\0';
                continue;
            }
            else
            {
                for(list<string>::iterator it=restru.begin(),end=restru.end();it!=end;++it)
                {
                    ofs << *it << endl;
                }
                ofs << endl;
                preinst = *(restru.end());
                restru.clear();
            }
        }
        else
        {
            ofs << s << endl;
        }
       ifs.getline(s,999);
    }
    ifs.close();

    return 0;
}
