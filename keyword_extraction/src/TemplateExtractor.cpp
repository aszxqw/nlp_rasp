#include "TemplateExtractor.h"

TemplateExtractor::TemplateExtractor()
{
}

TemplateExtractor::~TemplateExtractor()
{
}

bool TemplateExtractor::_matchTplUnit(const string& src, const string& tpl)
{
    size_t lenS = src.size();
    size_t lenT = tpl.size();
    if(0 == lenT || 0 == lenS)
    {
        string msg = "tpl:";
        msg += tpl;
        msg += " or src:";
        msg += src;
        msg += " is illegal";
        LogFatal(msg);
        return false;
    }
    if(1 == lenT && tpl[0] == '*')
    {
        return true;
    }
    size_t pos = tpl.find(src);
    if(pos == string::npos)
    {
        return false;
    }
    if((0 == pos || tpl[pos - 1] == '|') && (pos + lenS >= lenT || tpl[pos + lenS] == '|'))
    {
        return true;
    }
    else
    {
        return false;
    }
}


string TemplateExtractor::_matchTpl(const SS_PAIR_VEC& wordtags, const size_t beginPos, const SS_PAIR_VEC& tplPairVec)
{
    vector<string> res;
    //cout<<__LINE__<<",beginPos="<<beginPos<<endl;
    size_t j = 0;
    while(j < tplPairVec.size())
    {
        string wordTpl = tplPairVec[j].first;
        string tagTpl = tplPairVec[j].second;
        bool flag = false;
        for(size_t pos = j + beginPos; pos < wordtags.size(); pos++)
        {
            //cout<<__LINE__<<",j="<<j<<endl;
            string upword = upperStr(wordtags[pos].first);
            string tag = wordtags[pos].second;
            //cout<<__LINE__<<upword<<","<<tplPairVec[j].first<<endl;
            bool wordFlag = _matchTplUnit(upword, wordTpl);
            bool tagFlag = _matchTplUnit(tag, tagTpl);
            if(!wordFlag || !tagFlag)
            {
                break;
            }
            else
            {
                flag = true;
                res.push_back(wordtags[pos].first);
            }
        }
        if(!flag)
        {
            break;
        }
        j++;
    }
    //PRINT_VECTOR(res);
    //getchar();
    if(j == tplPairVec.size())
    {
        return joinStr(res, " ");
    }
    else
    {
        return "";
    }
}

string TemplateExtractor::_extract(const SS_PAIR_VEC& wordtags, const SS_PAIR_VEC& wordtagtpls)
{
    string res = "";
    FOR_VECTOR(wordtags, i)
    {
        string ret = _matchTpl(wordtags, i, wordtagtpls);
        if(0 < ret.size())
        {
            res = ret;
            break;
        }
    }
    return res;
}

void TemplateExtractor::run(SentenceBlockSt& st)
{
    const SS_PAIR_VEC& wordtags = st.wordtags;
    FOR_VECTOR(st.infoNames, i)
    {
        InfoTemplateSt* tpl = _findInfoTemplate(st.infoNames[i]);
        const SS_PAIR_VEC& wordtagtpls = tpl->tplPairVec;
        string infoValue = _extract(wordtags, wordtagtpls);
        st.infoValues.push_back(infoValue);
    }
}

void TemplateExtractor::init()
{
    _initInfoTemplateVec(g_INFO_TEMPLATE_CONF);
}

void TemplateExtractor::_initInfoTemplateVec(const string& confFilePath)
{
    if(!checkFileExist(confFilePath.c_str()))
    {
        LogFatal("init failed! can not find conf file");
        exit(1);
    }
    fstream ifile(confFilePath.c_str(), ios::in);
    string strBuf;
    vector<string> vecBuf;
    while(getline(ifile, strBuf))
    {
        //getchar();
        vecBuf.clear();
        splitStr(strBuf, vecBuf, "\t");
        if(vecBuf.size() != 2)
        {
            LogFatal("init failed! conf file's content is illeage!");
            exit(1);
        }
        InfoTemplateSt st;
        st.name = vecBuf[0];
        st.tplStr = vecBuf[1];
        vecBuf.clear();
        splitStr(st.tplStr, vecBuf, "/");
        //PRINT_VECTOR(vecBuf);
        //getchar();
        FOR_VECTOR(vecBuf, i)
        {
            vector<string> sVec;
            splitStr(vecBuf[i], sVec, ":");
            if(2 == sVec.size())
            {
                st.tplPairVec.push_back(pair<string, string>(sVec[0], sVec[1]));
            }
            else
            {
                LogFatal("init failed! conf file's content is illeage!");
                exit(1);
            }
        }
        //printInfoTemplateSt(st);
        //getchar();
        _infoTplVec.push_back(st);
    }
    ifile.close();
}

/*void TemplateExtractor::mapKeyInfo(SentenceBlockSt& st)
  {
  ServiceEventSt* ptSt = _findServiceEvent(st.serviceName);
  const vector<string>& vec = ptSt->keyInfoVec;
  FOR_VECTOR(vec, i)
  {
  string infoTplName = vec[i];
  InfoTemplateSt* p = _findInfoTemplate(infoTplName);
  if(NULL != p)
  {
  _keyInfoExt(st.wordtags, p->tplPairVec);
  }
  }
  }*/

/*void TemplateExtractor::_keyInfoExt(const SS_PAIR_VEC& wordtags, const SS_PAIR_VEC& tplPairVec)
{
    FOR_VECTOR(tplPairVec, i)
    {
        cout<<tplPairVec[i].first<<tplPairVec[i].second<<endl;
    }

}*/

InfoTemplateSt* TemplateExtractor::_findInfoTemplate(const string& name)
{
    FOR_VECTOR(_infoTplVec, i)
    {
        if(name == _infoTplVec[i].name)
        {
            return &_infoTplVec[i];
        }
    }
    return NULL;
}

#ifdef TEMPLATE_EXTRACTOR_UT
int main()
{
    SentenceBlockSt sb;
    sb.wordtags.push_back(pair<string, string>("reserve", "VV0"));
    sb.wordtags.push_back(pair<string, string>("ticket", "NN1"));
    sb.wordtags.push_back(pair<string, string>("from", "II"));
    sb.wordtags.push_back(pair<string, string>("Lowa", "NP1"));
    sb.wordtags.push_back(pair<string, string>("to", "II"));
    sb.wordtags.push_back(pair<string, string>("LA", "NP1"));
    sb.wordtags.push_back(pair<string, string>("airport", "NNL1"));
    sb.wordtags.push_back(pair<string, string>("at", "II"));
    sb.wordtags.push_back(pair<string, string>("25th", "MD"));
    sb.wordtags.push_back(pair<string, string>("August", "NPM1"));
    sb.serviceName = "__ReserveAirTicket__";
    sb.infoNames.push_back("__LocationFrom__");
    sb.infoNames.push_back("__LocationTo__");
    sb.infoNames.push_back("__Date__");

    /*sb.verbs.push_back(pair<string, string>("reserve", "VV0"));
      sb.nouns.push_back(pair<string, string>("ticket", "NN1"));
      sb.nouns.push_back(pair<string, string>("Lowa", "NP1"));
      sb.nouns.push_back(pair<string, string>("LA", "NP1"));
      sb.nouns.push_back(pair<string, string>("airport", "NNL1"));
      */
    TemplateExtractor te;
    te.init();
    printSentenceBlockSt(sb);
    te.run(sb);
    printSentenceBlockSt(sb);

    return 0;
}
#endif
