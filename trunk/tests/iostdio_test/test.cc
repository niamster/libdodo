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
		stdio *io = dynamic_cast<stdio *>(st->executor);
		/* io->outSize = 100; */ /* FIXME: issue #61 */
		/* io->writeStream("\nhook\n"); */

		dodoString str = ">" + st->buffer + "<\n";

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
		st.addXExec(XEXEC_ACTION_PREEXEC, ::hook, NULL);
#endif

		st.outSize = sizeof("write");
		st.write("write");
		st.flush();

#ifndef IO_WO_XEXEC
		st.disableXExecs = true;
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
