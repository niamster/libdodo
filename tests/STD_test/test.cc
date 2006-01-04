#include <flushSTD.h>
#include <flushSocketTools.h>

using namespace dodo;
using namespace std;

void 
hook(void *base,
	xexecObjTypeEnum type, 
	void *yep)
{
	flushSTD *st = (flushSTD *)base;
	if (st->operType == FLUSHSTD_OPER_WRITE)
	{
		char q[100];
		int *a;
		a = (int *)(st->buffer.c_str());
		sprintf(q,"!!%d!!\n",*a);
		st->buffer.assign(q);
	}
}

int main(int argc, char **argv)
{

	try
	{
		cout << flushSocketTools::getHostInfo("niam.mu").name << endl;
		cout << flushSTD::inputterInfo().host << endl;
		
		flushSTD st;
//		st.block(true);
		int pos = st.addPreExec(&hook,NULL);
		//st.outSize = 7;
		//st.autoOutSize = false;
		//st.outSTDBuffer = 2;
	
		int a = 10;
		st.write((char *)&a);
		st.flush();
		
		string o;
		
		st.inSize = 33;
		st.inSTDBuffer = 200;
		
		st.readStreamString(o);
		
		st.disablePreExec(pos);
		st.writeStreamString(o);
	}
	catch(baseEx ex)
	{
		cout << ex << "\t" << ex.line << endl;
	}
	return 0;
}
