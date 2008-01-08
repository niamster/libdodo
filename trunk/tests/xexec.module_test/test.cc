#include <libdodo/baseEx.h>
#include <libdodo/ioSTD.h>
#include <libdodo/directives.h>

#include <iostream>

using namespace dodo;
using namespace std;

int main(int argc, char **argv)
{

	try
	{
		ioSTD st;

		st.outSize = sizeof(int);
	
		#ifdef DL_EXT
		cout << ioSTD::getModuleInfo("./module").name;
		
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
