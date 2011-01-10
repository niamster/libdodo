/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;
using namespace io;
using namespace tools;

using namespace std;

int
main(int  argc UNUSED,
     char **argv UNUSED)
{
    dodo_try {
        io::memory io0;

        io0.writeString("!12345890#!!@\n");
        io0.writeString("!12345890-!!@\n");
        io0.writeString("!12345890@!!@\n");
        io0.writeString("!12345890$!!@\n");

        io::memory io1 = io0;

        io1.pos = 14;
        io1.bs = 14;
        io1.write("!12345890$!~@\n");

        /**
         * io should contain
           !12345890#!!@
           !12345890$!~@
           !12345890@!!@
           !12345890$!!@
         */

        dodo::string str;

        io1.pos = 2*14;
        str = io1.readString();
        cout << "\nSize: " << str.size() << endl;
        cout << "~~" << str << "~~" << endl << endl;

        io1.pos = 2*14;
        str = io1.read();
        cout << "\nSize: " << str.size() << endl;
        cout << "~~" << str << "~~" << endl << endl;

        cout << "io0:" << endl;
        cout << io0 << endl;
        cout << "io1:" << endl;
        cout << io1 << endl;
    } dodo_catch (dodo::exception::basic *e)   {
        cout << (dodo::string)*e << "\t" << e->line << "\t" << e->file << endl;
    }

    return 0;
}
