#ifndef TAGGERCLIENT_H
#define TAGGERCLIENT_H
#include <Ice/Ice.h>
#include "TaggerICE.h"
#include "cppcommon/io_functs.h"
#include "cppcommon/vec_functs.h"
#include "globals.h"
#include <string>
using namespace std;
using namespace RaspICE;
class TaggerClient
{
	public:
		TaggerClient();
		~TaggerClient();
    public:
		string run(string s);
        void init();
	private:
        Ice::CommunicatorPtr _communicatorPtr;
        TaggerICEPrx _taggerICEPrx;
		
};
#endif
