#ifndef TAGGERICESERVER_H
#define TAGGERICESERVER_H

#include <Ice/Ice.h>
#include "TaggerICE.h"
#include <string>
#include "Tagger.h"
using namespace std;
using namespace RaspICE;

class TaggerICEServer: public TaggerICE 
{
	public:
		TaggerICEServer();
		~TaggerICEServer();
	public:
		//demo
		//virtual void printString(const string& s, const Ice::Current&);
		//virtual string getString(const Ice::Current&);
		
		virtual string RunFromString(const string& s, const Ice::Current&);

	private:
		Tagger _tagger;
};

#endif
