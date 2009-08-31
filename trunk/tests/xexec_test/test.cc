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

int main(int argc, char **argv)
{
	try
	{
		stdio io;
#ifdef DL_EXT
#ifndef IO_WO_XEXEC
		int preExecId, postExecId;
		cout << stdio::module("./module").name << endl;

		io.addXExec("./module", NULL, preExecId, postExecId);

		io.removeXExec(preExecId);

		io.addXExec("./module", NULL, preExecId, postExecId);
		io.addXExec("./module", NULL, preExecId, postExecId, (void *)"perform");
#endif
#endif

		io.outSize = sizeof("xexec");
		io.write("xexec");
		io.flush();
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << endl;
	}
	return 0;
}
