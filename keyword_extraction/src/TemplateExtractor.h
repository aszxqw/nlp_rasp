#ifndef TEMPLATE_EXTRACTOR_H
#define TEMPLATE_EXTRACTOR_H

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <set>
#include <fstream>
#include <cstdlib>
#include "cppcommon/logger.h"
#include "cppcommon/str_functs.h"
#include "cppcommon/file_functs.h"
#include "cppcommon/vec_functs.h"
#include "structs.h"
#include "globals.h"
#include "typedefs.h"

using namespace CPPCOMMON;
using namespace std;

struct InfoTemplateSt
{
    string name;
    string tplStr;
    SS_PAIR_VEC tplPairVec;
};

inline void printInfoTemplateSt(const InfoTemplateSt& st)
{
    cout<<"[name:"<<st.name<<"]"<<endl;
    cout<<"[tplStr:"<<st.tplStr<<"]"<<endl;
    cout<<"[tplPairVec:"<<st.tplPairVec.size()<<"]"<<endl;
    FOR_VECTOR(st.tplPairVec, i)
    {
        cout<<st.tplPairVec[i].first<<","<<st.tplPairVec[i].second<<endl;
    }
}

class TemplateExtractor
{
    public:
        TemplateExtractor();
        ~TemplateExtractor();
    public:
        //void mapKeyInfo(SentenceBlockSt& st);
        void init();
        void run(SentenceBlockSt& st);
    private:
        void _initInfoTemplateVec(const string& confFilePath);
        InfoTemplateSt* _findInfoTemplate(const string& name);
        //void _keyInfoExt(const SS_PAIR_VEC&, const SS_PAIR_VEC&);
        bool _matchTplUnit(const string& src, const string& tpl);
        string _matchTpl(const SS_PAIR_VEC& wordtags, const size_t beginPos, const SS_PAIR_VEC& tplPairVec);
        string _extract(const SS_PAIR_VEC& , const SS_PAIR_VEC& );
    private:
        vector<InfoTemplateSt> _infoTplVec;


};

#endif
