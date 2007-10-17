#include <libdodo/baseEx.h>
#include <libdodo/flushSTD.h>
#include <libdodo/flushSocketTools.h>

#include <iostream>

using namespace dodo;
using namespace std;

void 
hook(void *base,
	short int type, 
	void *yep)
{
	flushSTD *st = (flushSTD *)base;
	if (st->operType == FLUSHSTD_OPER_WRITE)
	{
		char q[100];
		int *a;
		a = (int *)(st->buffer.c_str());
		sprintf(q,"%d",*a);
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
		//st.block(true);
		int pos = st.addPreExec(&hook,NULL);
		//st.outSTDBuffer = 2;
	
		st.outSize = sizeof(int);
	
		int a = 10;
		st.write((char *)&a);
		st.flush();
		
		dodoString o;
		
		st.inSize = 33;
		st.inSTDBuffer = 20;
		
		st.readString(o);
		
		std::cout << o.size() << "\n";
		
		st.disablePreExec(pos);
		st.writeStreamString(o);
		st.writeStreamString("\nexiting\n");
	}
	catch(baseEx ex)
	{
		cout << (string)ex << "\t" << ex.line << endl;
	}
	
	return 0;
}
