#include "io_functs.h"

namespace CPPCOMMON
{
	string loadFile2Str(const char * const filepath)
	{
		ifstream in(filepath, ios::in);
		istreambuf_iterator<char> beg(in), end;
		string str(beg, end);
		in.close();
		return str;
	}
}

#ifdef TEST_IO_FUNCTS
#include <iostream>
using namespace CPPCOMMON;
using namespace std;
int main()
{
	char filename[] = "1/2/3";
	cout<<loadFile2Str("1")<<endl;
	return 0;
}
#endif
