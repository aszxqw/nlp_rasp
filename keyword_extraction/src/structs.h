#ifndef STRUCTS_H
#define STRUCTS_H
#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <map>
#include <fstream>
#include "cppcommon/vec_functs.h"
#include "typedefs.h"
using namespace std;

struct SentenceBlockSt
{
	string logicalWord;
	string sentence;
	SS_PAIR_VEC wordtags;
	SS_PAIR_VEC nouns;
	SS_PAIR_VEC verbs;
	string serviceName;
	vector<string> infoNames;
    vector<string> infoValues;
	//map<string, string> infoMap;
};

inline void printSentenceBlockSt(const SentenceBlockSt& st)
{
	cout<<"******************************************"<<endl;
	cout<<"[logicalWord:"<<st.logicalWord<<"]"<<endl;
	cout<<"[sentence:"<<st.sentence<<"]"<<endl;
	cout<<"[wordtags:"<<st.wordtags.size()<<"]"<<endl;
	FOR_VECTOR(st.wordtags, i)
	{
		cout<<st.wordtags[i].first<<","<<st.wordtags[i].second<<endl;
	}
	cout<<"[verbs:"<<st.verbs.size()<<"]"<<endl;
	FOR_VECTOR(st.verbs, i)
	{
		cout<<st.verbs[i].first<<","<<st.verbs[i].second<<endl;
	}
	cout<<"[nouns:"<<st.nouns.size()<<"]"<<endl;
	FOR_VECTOR(st.nouns, i)
	{
		cout<<st.nouns[i].first<<","<<st.nouns[i].second<<endl;
	}
	cout<<"[serviceName:"<<st.serviceName<<"]"<<endl;
	cout<<"[infoNames:"<<st.infoNames.size()<<"]"<<endl;
	PRINT_VECTOR(st.infoNames);
	cout<<"[infoValues:"<<st.infoValues.size()<<"]"<<endl;
	PRINT_VECTOR(st.infoValues);
	cout<<"******************************************"<<endl;
}

#endif
