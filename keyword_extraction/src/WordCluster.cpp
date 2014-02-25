#include "WordCluster.h"

void WordCluster::_initServiceEventVec(const string& confFilePath)
{
	if(SERVICE_EVENT_INIT_LEVEL != _initLevel)
	{
		LogFatal("init failed!");
		exit(1);
	}
	if(!checkFileExist(confFilePath.c_str()))
	{
		LogFatal("init failed! can not find conf file");
		exit(1);
	}
	fstream ifile(confFilePath.c_str(), ios::in);
	string strBuf;
	while(getline(ifile, strBuf))
	{
		//cout<<strBuf<<endl;
		//getchar();
		vector<string> tmp;
		splitStr(strBuf, tmp, "\t");
		if(tmp.size() != 2)
		{
			LogFatal("init failed! conf file's content is illeage!");
			exit(1);
		}
		ServiceEventSt st;
		st.name = tmp[0];
		string keyInfoStr = tmp[1];
		tmp.clear();
		splitStr(keyInfoStr, tmp, "&");
		if(tmp.size() < 1)
		{
			LogFatal("init failed! conf file's content is illeage!");
			exit(1);
		}
		FOR_VECTOR(tmp, i)
		{
			//cout<<tmp[i]<<endl;
			//getchar();
			st.keyInfoVec.push_back(tmp[i]);
		}
		_serviceEventVec.push_back(st);
	}
	ifile.close();
}

void WordCluster::_initWordClusterVec(const string& confFilePath)
{
	if(WORD_CLUSTER_INIT_LEVEL != _initLevel)
	{
		LogFatal("init failed!");
		exit(1);
	}
	if(!checkFileExist(confFilePath.c_str()))
	{
		LogFatal("init failed! can not find conf file");
		exit(1);
	}
	fstream ifile(confFilePath.c_str(), ios::in);
	string strBuf;
	while(getline(ifile, strBuf))
	{
		SynonymsClusterSt wordClt;
		vector<string> tmp;
		splitStr(strBuf, tmp, "\t");
		if(tmp.size() != 3)
		{
			LogFatal(string("init failed! conf file's content is illeage! line:") + strBuf);
			exit(1);
		}
		wordClt.name = tmp[0];
		wordClt.kind = atoi(tmp[1].c_str());
		vector<string> eventNames;
		splitStr(tmp[2], eventNames, "|");
		FOR_VECTOR(eventNames, i)
		{
			ServiceEventSt* ptSE = _findServiceEvent(eventNames[i]);
			if(NULL != ptSE)
			{
				wordClt.eventPtVec.push_back(ptSE);
			}
		}
		_wordClusterVec.push_back(wordClt);
	}
	ifile.close();
}

void WordCluster::_initInputWordVec(const string& confFilePath)
{
	if(INPUT_WORD_INIT_LEVEL != _initLevel)
	{
		LogFatal("init failed!");
		exit(1);
	}
	if(!checkFileExist(confFilePath.c_str()))
	{
		LogFatal("init failed! can not find conf file");
		exit(1);
	}
	fstream ifile(confFilePath.c_str(), ios::in);
	string strBuf;
	while(getline(ifile, strBuf))
	{
		InputWordSt inwords;
		vector<string> tmp;
		splitStr(strBuf, tmp, "\t");
		if(tmp.size() != 3)
		{
			LogFatal("init failed! conf file's content is illeage!");
			exit(1);
		}
		inwords.word = tmp[0];
		inwords.wordKind = atoi(tmp[1].c_str());
		vector<string> clusterNames;
		splitStr(tmp[2], clusterNames, "|");
		FOR_VECTOR(clusterNames, i)
		{
			SynonymsClusterSt* ptWC = _findWordCluster(clusterNames[i]);
			//cout<<clusterNames[i]<<endl;
			if(NULL != ptWC)
			{
				inwords.clusterPtVec.push_back(ptWC);
				//cout<<ptWC->name<<endl;
			}
		}
		_inputWordVec.push_back(inwords);
	}
	ifile.close();
}

WordCluster::WordCluster()
{
}


WordCluster::~WordCluster()
{

}

void WordCluster::display()
{
	
}

void WordCluster::init()
{
	LogInfo("WordCluster Init Start ..");
	//_initLevel = INFO_TPL_INIT_LEVEL;
	//_initInfoTemplateVec(g_INFO_TEMPLATE_CONF);
	_initLevel = SERVICE_EVENT_INIT_LEVEL;
	_initServiceEventVec(g_SERVICE_EVENT_CONF);
	_initLevel = WORD_CLUSTER_INIT_LEVEL;
	_initWordClusterVec(g_WORD_CLUSTER_CONF);
	_initLevel = INPUT_WORD_INIT_LEVEL;
	_initInputWordVec(g_WORD_INPUT_CONF);
	LogInfo("WordCluster Init OK ..");
}


int WordCluster::_countSimilarity(const InputWordSt& A, const InputWordSt& B)
{
	string wordA = upperStr(A.word);
	string wordB = upperStr(B.word);
	if(0 != A.wordKind && 0 != B.wordKind && B.wordKind != A.wordKind)
	{
		return 0;
	}
	return countStrSimilarity(wordA, wordB) - countStrDistance(wordA, wordB);
}


InputWordSt* WordCluster::_wordInputMatch(const string& word, UINT kind)
{
	InputWordSt inword;
	inword.word = word;
	inword.wordKind = kind;
	

	InputWordSt* res = NULL;
	int score = 0;
	FOR_VECTOR(_inputWordVec, i)
	{
		int ret = _countSimilarity(inword, _inputWordVec[i]);
		if(ret > score)
		{
			score = ret;
			res = &_inputWordVec[i];
		}
	}
	return res;
}

SynonymsClusterSt* WordCluster::_findWordCluster(const string& name)
{
	FOR_VECTOR(_wordClusterVec, i)
	{
		if(name == _wordClusterVec[i].name)
		{
			return &_wordClusterVec[i];
		}
	}
	return NULL;
}



ServiceEventSt* WordCluster::_findServiceEvent(const string& name)
{
	FOR_VECTOR(_serviceEventVec, i)
	{
		if(name == _serviceEventVec[i].name)
		{
			return &_serviceEventVec[i];
		}
	}
	return NULL;
}


vector<InputWordSt* > WordCluster::_mapInputWords(const SentenceBlockSt& SBst)
{
	vector<InputWordSt* > res;
	FOR_VECTOR(SBst.verbs, i)
	{
		InputWordSt* ret = _wordInputMatch((SBst.verbs[i]).first, VERB);
		if(NULL != ret)
		{
			//cout<<ret->word<<endl;
			res.push_back(ret);
		}
	}
	FOR_VECTOR(SBst.nouns, i)
	{
		InputWordSt* ret = _wordInputMatch((SBst.nouns[i]).first, NOUN);
		if(NULL != ret)
		{
			//cout<<ret->word<<endl;
			res.push_back(ret);
		}
	}
	return res;
}

WordClusterScorePairVec WordCluster::_mapWordClusters(const vector<InputWordSt* >& inputWords)
{
	WordClusterScorePairVec res;
	map<SynonymsClusterSt*, UINT> mp;
	FOR_VECTOR(inputWords, i)
	{
		const vector<SynonymsClusterSt* >& nexts = inputWords[i]->clusterPtVec;
		FOR_VECTOR(nexts, j)
		{
			mp[nexts[j]]++;
		}
	}
	for(map<SynonymsClusterSt*, UINT>::iterator it = mp.begin(); it != mp.end(); it++)
	{
		res.push_back(*it);
	}
	return res;
}

ServiceEventScorePairVec WordCluster::_mapServiceEvents(const WordClusterScorePairVec& wordClusters)
{
	ServiceEventScorePairVec res;
	map<ServiceEventSt*, UINT> mp;
	FOR_VECTOR(wordClusters, i)
	{
		const vector<ServiceEventSt* >& nexts = wordClusters[i].first->eventPtVec;
		UINT score = wordClusters[i].second;
		FOR_VECTOR(nexts, j)
		{
			mp[nexts[j]] += score;
		}
	}
	for(map<ServiceEventSt*, UINT>::iterator it = mp.begin(); it != mp.end(); it++)
	{
		res.push_back(*it);
	}
	return res;
}

/*void WordCluster::mapKeyInfo(SentenceBlockSt& st)
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

/*void WordCluster::_keyInfoExt(const SS_PAIR_VEC& wordtags, const SS_PAIR_VEC& tplPairVec)
{
	FOR_VECTOR(tplPairVec, i)
	{
		cout<<tplPairVec[i].first<<tplPairVec[i].second<<endl;
	}
	
}*/

void WordCluster::run(SentenceBlockSt& st)
{
	ServiceEventSt* p = mapServiceEvent(st);

	if(NULL != p)
	{
		st.serviceName = p->name;
		st.infoNames = p->keyInfoVec;
	}
	else
	{
		LogError("mapServiceEvent return Null");
	}
}

ServiceEventSt* WordCluster::mapServiceEvent(const SentenceBlockSt& SBst)
{
	char strBuf[10];
	vector<InputWordSt*> inputWords = _mapInputWords(SBst);
	if(inputWords.empty())
	{
		LogError("_mapInputWords return empty.");
		return NULL;
	}

	LogDebug("print _mapInputWords result:");
	FOR_VECTOR(inputWords, i)
	{
		string msg = inputWords[i]->word;
		LogDebug(msg);
	}
	LogDebug("print end");

	WordClusterScorePairVec wcsPairVec = _mapWordClusters(inputWords);
	if(wcsPairVec.empty())
	{
		LogError("_mapWordClusters return empty.");
		return NULL;
	}
	LogDebug("print _mapWordClusters result:");
	FOR_VECTOR(wcsPairVec, i)
	{
		string msg = wcsPairVec[i].first->name;
		msg += ",";
		//itoa(wcsPairVec[i].second, strBuf, sizeof(strBuf));
		sprintf(strBuf, "%d", wcsPairVec[i].second);
		msg += strBuf;
		LogDebug(msg);
	}
	LogDebug("print end");

	ServiceEventScorePairVec sesPairVec = _mapServiceEvents(wcsPairVec);
	if(sesPairVec.empty())
	{
		LogError("_mapServiceEvents return empty.");
		return NULL;
	}
	LogDebug("print _mapServiceEvents result:");
	FOR_VECTOR(sesPairVec, i)
	{
		string msg = sesPairVec[i].first->name;
		msg += ",";
		//itoa(sesPairVec[i].second, strBuf, sizeof(strBuf));
		sprintf(strBuf, "%d", sesPairVec[i].second);
		msg += strBuf;
		LogDebug(msg);
	}
	LogDebug("print end");
	
	/*get the highest score serviceevent*/
	UINT score = 0;
	ServiceEventSt* res = NULL;
	FOR_VECTOR(sesPairVec, i)
	{
		UINT nscore = sesPairVec[i].second;
		if(nscore > score)
		{
			score = nscore;
			res = sesPairVec[i].first;
		}
	}
	return res;
}



#ifdef WORDCLUSTER_UT
using namespace std;
int main()
{
	WordCluster wc;
	wc.init();
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
	
	sb.verbs.push_back(pair<string, string>("reserve", "VV0"));
	sb.nouns.push_back(pair<string, string>("ticket", "NN1"));
	sb.nouns.push_back(pair<string, string>("Lowa", "NP1"));
	sb.nouns.push_back(pair<string, string>("LA", "NP1"));
	sb.nouns.push_back(pair<string, string>("airport", "NNL1"));
	wc.run(sb);
    printSentenceBlockSt(sb);

	return 0;
}
#endif
