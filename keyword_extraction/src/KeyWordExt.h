#ifndef KEYWORDEXT_H
#define KEYWORDEXT_H

#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <cstdio>
#include "structs.h"
#include "cppcommon/vec_functs.h"
#include "cppcommon/file_functs.h"
#include "cppcommon/logger.h"
#include "globals.h"
using namespace CPPCOMMON;
using namespace std;

class KeyWordExt
{
	public:
		KeyWordExt();
		~KeyWordExt();
	public:
		void run(SentenceBlockSt& senbk);
		void init();
		void fillNounWords(SentenceBlockSt& st);
		void fillVerbWords(SentenceBlockSt& st);
	private:
		void _initTagVec(const string&, vector<string>& );
	private:
		vector<string> _verbtags;
		vector<string> _nountags;
};

#endif
