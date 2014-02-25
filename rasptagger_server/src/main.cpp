#include "Tagger.h"
#include <iostream>
using namespace std;

const char * const IN_FNAME = "sample_input.txt";
const char * const SENTENCE_OUT_FNAME = "sentence.out";
const char * const TOKEN_OUT_FNAME = "token.out";
const char * const TAGGER_OUT_FNAME = "tagger.out";


int main(int argc, char *argv[])
{
	Tagger tagger;
	tagger.Init();
	tagger.RunFromFile("sample_input.txt", "tagger.out");
	tagger.RunFromFile("sample_input.txt", "tagger2.out");
	//cout<<tagger.RunFromString("sample_input.")<<endl;
	tagger.Clear();

	return 0;
}
