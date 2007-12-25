#include <libdodo/baseEx.h>
#include <libdodo/ioSTD.h>
#include <libdodo/ioSocketTools.h>

#include <iostream>

using namespace dodo;
using namespace std;

void 
hook(void *base,
	short int type, 
	void *yep)
{
	__xexexIoCollectedData *st = (__xexexIoCollectedData *)base;
	if (st->operType == IOSTD_OPERATION_WRITE)
	{
		char q[100];
		int a = *(int *)(st->buffer.c_str());
		++a;
		sprintf(q,"%d",a);
		st->buffer.assign(q);
	}
}

int main(int argc, char **argv)
{

	try
	{
		cout << ioSocketTools::getHostInfo("niam.mu").name << endl;
		cout << ioSTD::inputterInfo().host << endl;
		
		ioSTD st;
		int pos = st.addPreExec(&hook,NULL);
	
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
