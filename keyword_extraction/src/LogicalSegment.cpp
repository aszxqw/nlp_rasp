#include "LogicalSegment.h"

LogicalSegment::LogicalSegment()
{
	_condition_words.push_back("IF");
	_condition_words.push_back("ELSE");
	_condition_words.push_back("THEN");
	_condition_words.push_back("AND");
	_condition_words.push_back("OR");
}

LogicalSegment::~LogicalSegment()
{

}

SS_PAIR_VEC LogicalSegment::parseWordTagPairs(const string& s)
{
	SS_PAIR_VEC res;
	parseWordTagPairs(s, res);
	return res;
}

bool LogicalSegment::parseWordTagPairs(const string& s, SS_PAIR_VEC& wordtags)
{
	vector<string> now;
	_parseWordTags(s, now);
	for(size_t i = 0; i < now.size(); i++)
	{
		vector<string> tmp;
		splitStr(now[i], tmp, " ");
		if(tmp.size() == 2)
		{
			wordtags.push_back(make_pair<string, string>(tmp[0], tmp[1]));
		}
		else
		{
			cerr<<"split["<<now[i]<<"] failed!"<<endl;
			return false;
		}
	}
	return true;
}

bool LogicalSegment::_parseWordTags(const string& s, vector<string>& wordtags)
{
	splitStr(s, wordtags, "\n");
	return !wordtags.empty();
}

void LogicalSegment::init()
{
    
}

vector<SentenceBlockSt> LogicalSegment::run(const string& s)
{
	SS_PAIR_VEC wordtags = parseWordTagPairs(s);
	vector<SentenceBlockSt> SBs = cutSentenceBlock(wordtags);
	vector<SentenceBlockSt> retSBs;
	FOR_VECTOR(SBs, i)
	{
		if(isInVec(_condition_words, SBs[i].logicalWord))
		{
			retSBs.push_back(SBs[i]);
		}
	}
	return retSBs;
}

vector< pair<string, SS_PAIR_VEC > > LogicalSegment::_splitLogicalWords(const SS_PAIR_VEC& wordtags)
{
	vector< pair<string, SS_PAIR_VEC > > wordVecs;
	_splitLogicalWords(wordtags, wordVecs);
	return wordVecs;
}
bool LogicalSegment::_splitLogicalWords(const SS_PAIR_VEC& wordtags, vector< pair<string, SS_PAIR_VEC > >& outVec)
{
	SS_PAIR_VEC tmp;
	string pattern;
	size_t pSize = _condition_words.size();
	FOR_VECTOR(wordtags, i)
	{
		string word = wordtags[i].first;
		string tag = wordtags[i].second;
		string upcaseword = upperStr(word);
		//cout<<upcaseword<<endl;
		//getchar();
		size_t pPos = pSize;
		FOR_VECTOR(_condition_words, j)
		{
			if(_condition_words[j] == upcaseword)
			{
				pPos = j;
				break;
			}
		}
		if(pPos != pSize)
		{
			if(!tmp.empty())
			{
				outVec.push_back(make_pair<string, SS_PAIR_VEC>(pattern, tmp));
				tmp.clear();
			}
			pattern = _condition_words[pPos];
		}
		else
		{
			tmp.push_back(make_pair<string, string>(word, tag));
		}
		
	}
	if(!tmp.empty())
	{
		outVec.push_back(make_pair<string, SS_PAIR_VEC>(pattern, tmp));
	}
	return true;
}

vector<SentenceBlockSt> LogicalSegment::cutSentenceBlock(const SS_PAIR_VEC& wordtags)
{
	vector< pair<string, SS_PAIR_VEC > > wordVecs = _splitLogicalWords(wordtags);
	vector<SentenceBlockSt> retSBs;
	FOR_VECTOR(wordVecs, i)
	{
		SentenceBlockSt retSB;
		retSB.logicalWord = wordVecs[i].first;
		const SS_PAIR_VEC& sspv = wordVecs[i].second; 
		vector<string> words;
		FOR_VECTOR(sspv, j)
		{
			string word = sspv[j].first;
			string tag = sspv[j].second;
			words.push_back(word);
			retSB.wordtags.push_back(make_pair<string, string>(word, tag));
		}
		retSB.sentence = joinStr(words, " ");
		retSBs.push_back(retSB);
	}
	return retSBs;
}

#ifdef UINT_TEST
#include "cppcommon/io_functs.h"
#include "cppcommon/vec_functs.h"
int main()
{
	LogicalSegment lsm;
	string s = loadFile2Str("sample_output.txt");
	//vector< pair<string, string> > strs;
	//lsm.parseWordTagPairs(s, strs);
	vector<SentenceBlockSt> sbsts = lsm.run(s);
	FOR_VECTOR(sbsts, i)
	{
		printSentenceBlockSt(sbsts[i]);
	}
	return 0;
}
#endif
