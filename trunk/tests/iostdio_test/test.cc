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
hook(xexec::__collected_data__ *odata,
	 short int type,
	 void *udata)
{
	io::channel::__collected_data__ *st = (io::channel::__collected_data__ *)odata;
	if (st->operType == io::channel::OPERATION_WRITE)
	{
		stdio *io = dynamic_cast<stdio *>(st->executor);
		/* io->blockSize = 100; */ /* FIXME: issue #61 */
		/* io->writeString("\nhook\n"); */

		dodoString str = ">" + st->buffer + "<\n";

		io->blockSize = str.size();
		st->buffer.assign(str);
	}
}
#endif

int main(int argc, char **argv)
{

	try
	{
		stdio st;
		cout << st.peer().host << endl;

#ifndef IO_WO_XEXEC
		st.addXExec(xexec::ACTION_PREEXEC, ::hook, NULL);
#endif

		st.blockSize = sizeof("write");
		st.write("write");
		st.flush();

#ifndef IO_WO_XEXEC
		st.disableXExecs = true;
#endif
		dodoString o;

		st.blockSize = 33;

		o = st.read();

		cout << o.size() << "\n";
		cout << o << "\n";

		st.blockSize = 4;
		st.write("1234567890");
		st.writeString("\n");

		st.blockSize = 40;
		st.writeString(o);
		st.writeString("\nexiting\n");
	}
	catch (dodo::exception::basic &ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}
