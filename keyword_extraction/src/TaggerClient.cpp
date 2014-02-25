#include "TaggerClient.h"

TaggerClient::TaggerClient()
{
	
}

TaggerClient::~TaggerClient()
{

}


void TaggerClient::init()
{
	//Ice::CommunicatorPtr ic;
	//ic = Ice::initialize();
	_communicatorPtr = Ice::initialize();
	string strforproxy;
	strforproxy += g_ICE_IDENTITY_NAME;
	strforproxy += ":default -p 10000";
	Ice::ObjectPrx base = _communicatorPtr->stringToProxy(strforproxy);
	//TaggerICEPrx taggerICEPrx = TaggerICEPrx::checkedCast(base);
	_taggerICEPrx = TaggerICEPrx::checkedCast(base);
	if(!_taggerICEPrx)
	  throw "Invalid proxy";
}


string TaggerClient::run(string s)
{
	return _taggerICEPrx->RunFromString(s);
}

#ifdef UINT_TEST
using namespace CPPCOMMON;
int main()
{
	TaggerClient client;
	string s = loadFile2Str("sample_output.txt");
	cout<<client.run(s)<<endl;
	return 0;
}
#endif
