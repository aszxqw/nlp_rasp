#include <Ice/Ice.h>
#include "TaggerICE.h"
#include "cppcommon/io_functs.h"
using namespace std;
using namespace RaspICE;
using namespace CPPCOMMON;

int main(int argc, char * argv[])
{
	Ice::CommunicatorPtr ic;
	ic = Ice::initialize(argc, argv);
	Ice::ObjectPrx base = ic->stringToProxy("SimplePrinter:default -p 10000");
	TaggerICEPrx taggerICEPrx = TaggerICEPrx::checkedCast(base);
	if(!taggerICEPrx)
	  throw "Invalid proxy";
	//taggerICEPrx->printString("hello World!");
	
	string s = loadFile2Str("sample_input.txt");
	cout<<taggerICEPrx->RunFromString(s)<<endl;
}
