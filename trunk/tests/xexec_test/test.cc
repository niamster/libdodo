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
		stdio st;

		st.outSize = sizeof(int);

#ifdef DL_EXT
#ifndef IO_WO_XEXEC
		int preExecId, postExecId;
		cout << stdio::getModuleInfo("./module").name << endl;

		st.addXExec("./module", NULL, preExecId, postExecId);

		st.delXExec(preExecId);

		st.addXExec("./module", NULL, preExecId, postExecId);
		st.addXExec("./module", NULL, preExecId, postExecId, (void *)"perform");
#endif
#endif

		int a = 10;
		st.write((char *)&a);
		st.flush();
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << endl;
	}
	return 0;
}
