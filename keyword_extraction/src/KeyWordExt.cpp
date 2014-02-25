#include "KeyWordExt.h"

KeyWordExt::KeyWordExt()
{
}

KeyWordExt::~KeyWordExt()
{

}

void KeyWordExt::init()
{
	_initTagVec(g_VERB_TAGS_CONF, _verbtags);
	_initTagVec(g_NOUN_TAGS_CONF, _nountags);
}

void KeyWordExt::_initTagVec(const string& confFilePath, vector<string>& tagVec)
{
	if(!checkFileExist(confFilePath.c_str()))
	{
		LogFatal("init failed! can not find conf file");
		exit(1);
	}
	fstream ifile(confFilePath.c_str(), ios::in);
	string strBuf;
	while(getline(ifile, strBuf))
	{
		tagVec.push_back(strBuf);
	}
	ifile.close();
}

void KeyWordExt::fillNounWords(SentenceBlockSt& st)
{
	const SS_PAIR_VEC& wordtags = st.wordtags;
	FOR_VECTOR(wordtags, i)
	{
		string word = wordtags[i].first;
		string tag = wordtags[i].second;
		if(isInVec(_nountags, tag))
		{
			st.nouns.push_back(make_pair<string, string>(word, tag));
		}
	}
}

void KeyWordExt::fillVerbWords(SentenceBlockSt& st)
{
	const SS_PAIR_VEC& wordtags = st.wordtags;
	FOR_VECTOR(wordtags, i)
	{
		string word = wordtags[i].first;
		string tag = wordtags[i].second;
		if(isInVec(_verbtags, tag))
		{
			st.verbs.push_back(make_pair<string, string>(word, tag));
		}
	}
}

void KeyWordExt::run(SentenceBlockSt& senbk)
{
	fillNounWords(senbk);
	fillVerbWords(senbk);
}

#ifdef UINT_TEST
using namespace std;
int main()
{
	SentenceBlockSt sb;
	sb.logicalWord = "ELSE";
	sb.wordtags.push_back(make_pair<string, string>("reserve", "VV0"));
	sb.wordtags.push_back(make_pair<string, string>("a", "AT1"));
	sb.wordtags.push_back(make_pair<string, string>("ticket", "NN1"));
	sb.wordtags.push_back(make_pair<string, string>("from", "II"));
	sb.wordtags.push_back(make_pair<string, string>("Lowa", "NP1"));
	sb.wordtags.push_back(make_pair<string, string>("to", "II"));
	sb.wordtags.push_back(make_pair<string, string>("LA", "NP1"));
	sb.wordtags.push_back(make_pair<string, string>("airport", "NNL1"));
	sb.wordtags.push_back(make_pair<string, string>("and", "CC"));
	sb.wordtags.push_back(make_pair<string, string>("reserve", "VV0"));
	sb.wordtags.push_back(make_pair<string, string>("a", "AT1"));
	sb.wordtags.push_back(make_pair<string, string>("guesthouse", "NN1"));
	sb.wordtags.push_back(make_pair<string, string>("with", "IW"));
	sb.wordtags.push_back(make_pair<string, string>("twin", "NN1"));
	sb.wordtags.push_back(make_pair<string, string>("bed", "NN1"));
	sb.wordtags.push_back(make_pair<string, string>("from", "II"));
	sb.wordtags.push_back(make_pair<string, string>("18t", "MC"));
	sb.wordtags.push_back(make_pair<string, string>("August", "NPM1"));
	sb.wordtags.push_back(make_pair<string, string>("to", "II"));
	sb.wordtags.push_back(make_pair<string, string>("22nd", "MD"));
	sb.wordtags.push_back(make_pair<string, string>("August", "NPM1"));
	
	KeyWordExt kwe;
	kwe.run(sb);
	return 0;
}
#endif
