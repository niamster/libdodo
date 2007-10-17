#include <flushSTD.h>

using namespace dodo;
using namespace std;

int main(int argc, char **argv)
{

	try
	{
		flushSTD st;
		
		st.autoOutSize = false;
		st.outSize = sizeof(int);
		
		cout << flushSTD::getModuleInfo("./module").name;
		
		//int pos = st.addPreExec("./module",NULL);
	
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
		st.delPreExec(1);st.delPreExec(2);
		cout << st.addPreExec("./module",NULL)<<endl;
		cout << st.addPreExec("./module",NULL)<<endl;
		//st.delPreExec(pos);
	
		int a = 10;
		st.write((char *)&a);
		
		cout << endl << st.buffer << endl;
		
		st.flush();
	}
	catch(baseEx ex)
	{
		cout << ex << "\t" << ex.line << endl;
	}
	return 0;
}
