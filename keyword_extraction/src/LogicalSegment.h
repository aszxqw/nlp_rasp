#ifndef LOGICALSEGMENT_H
#define LOGICALSEGMENT_H
#include "cppcommon/str_functs.h"
#include "cppcommon/vec_functs.h"
#include "LogicalSegment.h"
#include <utility>
//#include <ext/hash_set>
#include "structs.h"
#include "typedefs.h"

using namespace std;
using namespace CPPCOMMON;
//using namespace __gnu_cxx;

class LogicalSegment
{
	public:
		LogicalSegment();
		~LogicalSegment();
	public:
        void init();
		vector<SentenceBlockSt> run(const string& s);
		vector<SentenceBlockSt> cutSentenceBlock(const vector< pair<string, string> >& wordtags);
		bool parseWordTagPairs(const string& s, vector< pair<string, string> >& wordtags);
		SS_PAIR_VEC parseWordTagPairs(const string& s);
	private:
		bool _splitLogicalWords(const SS_PAIR_VEC& wordtags, vector< pair<string, SS_PAIR_VEC > >& outVec);
		vector< pair<string, SS_PAIR_VEC > > _splitLogicalWords(const SS_PAIR_VEC& WORDtags);
		bool _parseWordTags(const string& s, vector<string>& wordtags);
	private:
		//hash_set<string> _wordset;
		vector<string> _condition_words;
		
};
#endif
