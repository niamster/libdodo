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
    cout << "baseHandler: " << (dodoString) * ex << endl << ex->backtrace() << endl;
}

int
main(int  argc UNUSED,
     char **argv UNUSED)
{
    try {
        dodo::exception::basic::setHandler(dodo::exception::MODULE_IOFILEREGULAR, &handler, NULL);
#ifdef DL_EXT
        dodo::exception::basic::setHandler("./module", NULL);
#endif

        try {
            tools::filesystem::unlink("some.file", false);
        } catch (...)    {
        }

        try {
            file::regular f;
            f.open("some.file", file::regular::OPEN_MODE_READ_ONLY);
        } catch (...)    {
        }

        dodoString string = "abcdef";
        dodoString original = string;
        tools::string::replace("cd", "CD", string);
        cout << "In " << original << " 'cd' replaced with 'CD': " << string << endl;

        dodoString random = tools::misc::stringRandom(12);
        cout << "Random:" << tools::code::MD5Hex(random) << endl;
    } catch (dodo::exception::basic &ex)   {
        cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
    } catch (std::exception &ex)   {
        cout << endl << ex.what() << endl;
    }

    return 0;
}

