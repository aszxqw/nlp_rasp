#include "TaggerICEServer.h"

TaggerICEServer::TaggerICEServer()
{
	_tagger.Init();
}

TaggerICEServer::~TaggerICEServer()
{
	_tagger.Clear();
}

//demo
/*void TaggerICEServer::printString(const string& s, const Ice::Current&)
{
	cout<<s<<endl;
}

string TaggerICEServer::getString(const Ice::Current&)
{
	return "hehe";
}*/

string TaggerICEServer::RunFromString(const string& s, const Ice::Current&)
{
	return _tagger.RunFromString(s);
}

int main(int argc, char * argv[])
{
	Ice::CommunicatorPtr ic;
	ic = Ice::initialize(argc, argv);
	Ice::ObjectAdapterPtr adapter = ic->createObjectAdapterWithEndpoints(
				g_ICE_ADAPTER_NAME, "default -p 10000");
	Ice::ObjectPtr object = new TaggerICEServer;
	adapter->add(object, ic->stringToIdentity(g_ICE_IDENTITY_NAME));
	adapter->activate();
	ic->waitForShutdown();
	return 0;
}
