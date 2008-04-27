#include <libdodo/baseEx.h>
#include <libdodo/ioSTD.h>
#include <libdodo/toolsNetwork.h>

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
		int a = *(int *)(st->buffer.c_str());
		
		ioSTD *io = (ioSTD *)st->executor;
		io->disableAll();
		io->writeStreamString("\nhook\n");
		io->enableAll();
                
                st->buffer.clear();

	
		++a;
		dodoString str = stringTools::iToString(a) + "\n";

		io->outSize = str.size();
		st->buffer.assign(str);
	}
}

int main(int argc, char **argv)
{

	try
	{
		ioSTD st;
		cout << st.inputterInfo().host << endl;

#ifndef IOSTD_WO_XEXEC
		int pos = st.addPreExec(&hook, NULL);
#endif
	
		st.outSize = sizeof(int);
	
		int a = 10;
		st.write((char *)&a);
		st.flush();
		
		dodoString o;
		
		st.inSize = 33;
		st.inSTDBuffer = 20;
		
		st.readString(o);
		
		cout << o.size() << "\n";
		
#ifndef IOSTD_WO_XEXEC
		st.disablePreExec(pos);
#endif
	
		st.outSize = 4;
		st.writeString("1234567890");
		st.writeString("\n");

		st.writeStreamString(o);
		st.writeStreamString("\nexiting\n");
	}
	catch(baseEx ex)
	{
		cout << (string)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}
	
	return 0;
}
