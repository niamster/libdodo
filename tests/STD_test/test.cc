#include <flushSTD.h>

using namespace dodo;
using namespace std;

void 
hook(dodoBase *base, 
	void *yep)
{
/*	flushSTD *st = dynamic_cast<flushSTD *>(base->getSelf());
	if (st->operType == FLUSHSTD_OPER_WRITE)
	{
		char q[100];
		int *a;
		a = (int *)(st->buffer.c_str());
		sprintf(q,"!!%d!!\n",*a);
		st->buffer.assign(q);
	}*/
}

int main(int argc, char **argv)
{

	try
	{
		cout << flushSocket::getHostInfo("niam.mu").name << endl;
		cout << flushSTD::inputterInfo().host << endl;
		
		flushSTD st;
		int pos = st.addPreExec(&hook,NULL);
		//st.outSize = 7;
		//st.autoOutSize = false;
		//st.outSTDBuffer = 2;
	
		int a = 10;
		st.write((char *)&a);
		st.flush();
		
		string o;
		
		st.inSize = 1;
		st.inSTDBuffer = 2;
		
		st.readString(o);
		
		st.disablePreExec(pos);
		st.writeString(o);
	}
	catch(baseEx ex)
	{
		cout << ex << "\t" << ex.line << endl;
	}
	return 0;
}
