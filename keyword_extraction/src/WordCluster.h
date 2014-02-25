#ifndef WORDCLUSTER_H
#define WORDCLUSTER_H

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

const UINT VERB = 1;
const UINT NOUN = 2;

const UINT SERVICE_EVENT_INIT_LEVEL = 1;
const UINT WORD_CLUSTER_INIT_LEVEL = 2; 
const UINT INPUT_WORD_INIT_LEVEL = 3;

/*struct InfoTemplateSt
{
	string name;
	string tplStr;
	SS_PAIR_VEC tplPairVec;
};*/

struct ServiceEventSt
{
	string name;
	vector<string> keyInfoVec;
	ServiceEventSt()
	{
		name = "";
	}
	ServiceEventSt(const string& _name)
	{
		name = _name;
	}
};

typedef vector< pair< ServiceEventSt*, UINT> > ServiceEventScorePairVec;

struct SynonymsClusterSt
{
	string name;
	UINT kind;
	/* 1:verb
	 * 2:noun
	 * */
	vector<ServiceEventSt* > eventPtVec;
	SynonymsClusterSt()
	{
		name = "";
		kind = 0;
	}
};

typedef vector< pair< SynonymsClusterSt*, UINT> > WordClusterScorePairVec;

struct InputWordSt
{
	string word;
	UINT wordKind;
	vector<SynonymsClusterSt* > clusterPtVec;
	InputWordSt()
	{
		word = "";
		wordKind = 0;
	}
};

class WordCluster
{
	public:
		WordCluster();
		~WordCluster();

	public:
		void init();
		void display();
		void run(SentenceBlockSt&);
		ServiceEventSt* mapServiceEvent(const SentenceBlockSt& );
		void mapKeyInfo(SentenceBlockSt& );
		
	private:
		void _initInfoTemplateVec(const string& confFilePath);
		void _initServiceEventVec(const string& confFilePath);
		void _initWordClusterVec(const string& confFilePath);
		void _initInputWordVec(const string& confFilePath);
	private:
		InputWordSt* _wordInputMatch(const string& word, UINT kind);
		int _countSimilarity(const InputWordSt& A, const InputWordSt& B);
		SynonymsClusterSt* _findWordCluster(const string& name);
		ServiceEventSt* _findServiceEvent(const string& name);
		//InfoTemplateSt* _findInfoTemplate(const string& name);
		//void _keyInfoExt(const SS_PAIR_VEC&, const SS_PAIR_VEC&);

	private:
		vector<InputWordSt* > _mapInputWords(const SentenceBlockSt& st);
		WordClusterScorePairVec _mapWordClusters(const vector<InputWordSt* >& inputWords);
		ServiceEventScorePairVec _mapServiceEvents(const WordClusterScorePairVec& wordClusters);

	private:
		//vector<InfoTemplateSt> _infoTplVec;
		vector<SynonymsClusterSt> _wordClusterVec;
		vector<ServiceEventSt> _serviceEventVec;
		vector<InputWordSt> _inputWordVec;
		UINT _initLevel;
};

#endif
