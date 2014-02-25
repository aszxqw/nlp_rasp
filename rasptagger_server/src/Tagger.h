#ifndef TAGGER_H
#define TAGGER_H
#include <string>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "globals.h"
#include "lib/mainl.h"
#include "cppcommon/io_functs.h"
using namespace CPPCOMMON;
using namespace std;

class Tagger
{
	private:
		static const char * const IN_FNAME;
		static const char * const SENTENCE_OUT_FNAME;
		static const char * const TOKEN_OUT_FNAME;
		static const char * const TAGGER_OUT_FNAME;
		static int TAGGER_ARGC;
		static char * TAGGER_ARGS[];
		
	public:
		void Init(int argc, char ** argv);
		void Init();
		void RunFromFile(const char * const in_fname, const char * out_fname);
		string RunFromString(const string & text);
		void Clear();

	private:
		//void _SentenceFromFile(const char * const in_fname, const char * const out_fname);
		//void _TokenFromFile(const char * const in_fname, const char * const out_fname);
		void _TokenFromFile(const char * const in_fname, const char * const out_fname);
		void _TaggingFromFile(const char * const in_fname, const char * out_fname);
		void _Tagging(FILE * in_file, FILE * out_file);

	private:
		int        iterations, initialise, iter, dict_size, ret;
		options_st saved_options;
		char       dictname[MAXFN], tranname[MAXFN], odictname[MAXFN], otranname[MAXFN], 
				   outname[MAXFN], mapname[MAXFN], skipname[MAXFN], reducename[MAXFN], 
				   infername[MAXFN], fsmname[MAXFN], grammarname[MAXFN], ukwname[MAXFN], 
				   bdbmname[MAXFN], runkstatname[MAXFN], wunkstatname[MAXFN];
		char       ofeaturesname[MAXFN], obadwordname[MAXFN];
		Dict       dict, skip_dict;
		Trans      trans, c_newtrans;
		//FILE       *corpfile, *outfile; /*input and output*/
		FILE       *odictfile, *otranfile;
		DB  *dbp;
		BOOL       any_output;
		

};

#endif
