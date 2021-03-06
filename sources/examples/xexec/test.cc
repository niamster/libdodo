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

int
main(int  argc UNUSED,
     char **argv UNUSED)
{
    dodo_try {
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

        io.bs = sizeof("xexec") - 1;
        io.write("xexec");
        io.flush();
    } dodo_catch (dodo::exception::basic *e)   {
        cout << (dodo::string)*e << "\t" << e->line << endl;
    }
    return 0;
}
