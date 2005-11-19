#include <flushSTD.h>

using namespace dodo;
using namespace std;

int main(int argc, char **argv)
{

	try
	{
		flushSTD st;
		int pos = st._addPreExecModule(&st,"./module",NULL);
	
		int a = 10;
		st.write((char *)&a);
		st.flush();
	}
	catch(baseEx ex)
	{
		cout << ex << "\t" << ex.line << endl;
	}
	return 0;
}
