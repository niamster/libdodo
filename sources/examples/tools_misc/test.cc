/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;

using namespace std;
using namespace io;

void
handler(int                    module UNUSED,
        dodo::exception::basic *ex,
        void                   *data UNUSED)
{
    cout << "baseHandler: " << (dodo::string)*ex << endl << ex->backtrace() << endl;
}

int
main(int  argc UNUSED,
     char **argv UNUSED)
{
    dodo_try {
        dodo::exception::basic::setHandler(dodo::exception::MODULE_IOFILEREGULAR, &handler, NULL);
#ifdef DL_EXT
        dodo::exception::basic::setHandler("./module", NULL);
#endif

		dodo::exception::basic *e;

        dodo_try {
            tools::filesystem::unlink("some.file", false);
        } dodo_catch (e)    {
        }

        dodo_try {
            file::regular f;
            f.open("some.file", file::regular::OPEN_MODE_READ_ONLY);
        } dodo_catch (e)    {
        }

        dodo::string string = "abcdef";
        dodo::string original = string;
        tools::string::replace("cd", "CD", string);
        cout << "In " << original << " 'cd' replaced with 'CD': " << string << endl;

        dodo::string random = tools::misc::stringRandom(12);
        cout << "Random:" << tools::code::MD5Hex(random) << endl;
    } dodo_catch (dodo::exception::basic *e)   {
        cout << (dodo::string)*e << "\t" << e->line << "\t" << e->file << endl;
    }

    return 0;
}

