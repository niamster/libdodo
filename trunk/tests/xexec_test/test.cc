#include <libdodo/baseEx.h>
#include <libdodo/ioStdio.h>
#include <libdodo/directives.h>

#include <iostream>

using namespace dodo;
using namespace io;

using namespace std;

int main(int argc, char **argv)
{

	try
	{
		stdio st;

		st.outSize = sizeof(int);
	
#ifdef DL_EXT
		cout << stdio::getModuleInfo("./module").name;
		
		cout << st.addPreExec("./module",NULL)<<endl;
		cout << st.addPreExec("./module",NULL)<<endl;
		cout << st.addPreExec("./module",NULL)<<endl;
		cout << st.addPreExec("./module",NULL)<<endl;
		cout << st.addPreExec("./module",NULL)<<endl;
		cout << st.addPreExec("./module",NULL)<<endl;
		cout << st.addPreExec("./module",NULL)<<endl;
		cout << st.addPreExec("./module",NULL)<<endl;
		cout << st.addPreExec("./module",NULL)<<endl;
		cout << st.addPreExec("./module",NULL)<<endl;
		st.delPreExec(1);
		st.delPreExec(2);
		cout << st.addPreExec("./module",NULL)<<endl;
		cout << st.addPreExec("./module",NULL,(void *)"perform")<<endl;
#endif
	
		int a = 10;
		st.write((char *)&a);
		
		st.flush();
	}
	catch(baseEx ex)
	{
		cout << (string)ex << "\t" << ex.line << endl;
	}
	return 0;
}
