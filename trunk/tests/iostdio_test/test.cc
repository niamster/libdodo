/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;
using namespace io;

using namespace std;

#ifndef IO_WO_XEXEC

void
hook(__xexecCollectedData__ *odata,
	 short int type,
	 void *udata)
{
	__xexecIoChannelCollectedData__ *st = (__xexecIoChannelCollectedData__ *)odata;
	if (st->operType == IO_OPERATION_WRITE)
	{
		int a = *(int *)(st->buffer.c_str());

		stdio *io = dynamic_cast<stdio *>(st->executor);
		io->outSize = 100;
		io->writeStream("\nhook\n");

		++a;
		dodoString str = tools::string::iToString(a) + "\n";

		io->outSize = str.size();
		st->buffer.assign(str);
	}
}

#endif

int main(int argc, char **argv)
{

	try
	{
		stdio st;
		cout << st.peerInfo().host << endl;

#ifndef IO_WO_XEXEC
		int pos = st.addXExec(XEXEC_ACTION_PREEXEC, ::hook, NULL);
#endif

		st.outSize = sizeof(int);

		int a = 10;
		st.write((char *)&a);
		st.flush();

#ifndef IO_WO_XEXEC
		st.disabledXExecs = true;
#endif
		dodoString o;

		st.inSize = 33;
		st.inPipeBuffer = 20;

		o = st.read();

		cout << o.size() << "\n";
		cout << o << "\n";

		st.outSize = 4;
		st.write("1234567890");
		st.writeStream("\n");

		st.outSize = 40;
		st.writeStream(o);
		st.writeStream("\nexiting\n");
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}
