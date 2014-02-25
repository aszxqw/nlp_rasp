#include "cppcommon/io_functs.h"
#include "cppcommon/logger.h"
#include "TaggerClient.h"
#include "LogicalSegment.h"
#include "KeyWordExt.h"
#include "WordCluster.h"
#include "TemplateExtractor.h"
using namespace std;
using namespace CPPCOMMON;

int main(int argc, char * argv[])
{
	LogInfo("program start ...");
	TaggerClient taggerClient;
	LogicalSegment logicalSegment;
	KeyWordExt keywordExt;
	WordCluster wordCluster;
    TemplateExtractor tplExt;
    taggerClient.init();
    logicalSegment.init();
	keywordExt.init();
	wordCluster.init();
    tplExt.init();

	string s = loadFile2Str("sample_input.txt");
	string res = taggerClient.run(s);
	vector<SentenceBlockSt>  SBs = logicalSegment.run(res);
	FOR_VECTOR(SBs, i)
	{
		keywordExt.run(SBs[i]);
		wordCluster.run(SBs[i]);
        tplExt.run(SBs[i]);
		printSentenceBlockSt(SBs[i]);
		getchar();
	}
    LogInfo("program end ...");
}
