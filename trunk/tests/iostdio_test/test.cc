/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/exceptionBasic.h>
#include <libdodo/ioStdio.h>
#include <libdodo/toolsNetwork.h>

#include <iostream>

using namespace dodo;
using namespace io;

using namespace std;

#ifndef IO_WO_XEXEC

void
hook(void *odata,
	 short int type,
	 void *udata)
{
	__xexecIoChannelCollectedData *st = (__xexecIoChannelCollectedData *)odata;
	if (st->operType == IO_OPERATION_WRITE)
	{
		int a = *(int *)(st->buffer.c_str());

		stdio *io = (stdio *)st->executor;
		io->disableAll();
		io->outSize = 100;
		io->writeStreamString("\nhook\n");
		io->enableAll();

		st->buffer.clear();


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

		int pos = st.addPreExec(&hook, NULL);

#endif

		st.outSize = sizeof(int);

		int a = 10;
		st.write((char *)&a);
		st.flush();

#ifndef IO_WO_XEXEC

		st.disablePreExec(pos);

#endif
		dodoString o;

		st.inSize = 33;
		st.inFifoBuffer = 20;

		st.readString(o);

		cout << o.size() << "\n";
		cout << o << "\n";

		st.outSize = 4;
		st.writeString("1234567890");
		st.writeStreamString("\n");

		st.outSize = 40;
		st.writeStreamString(o);
		st.writeStreamString("\nexiting\n");
	}
	catch (dodo::exception::basic ex)
	{
		cout << (string)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}
