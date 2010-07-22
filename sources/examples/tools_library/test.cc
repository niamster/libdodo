/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;

using namespace std;

#ifdef DL_EXT
using namespace tools;

typedef void (*print)(const dodo::string &data);
#endif

int
main(int  argc UNUSED,
     char **argv UNUSED)
{
    dodo_try {
#ifdef DL_EXT
#ifdef BFD_EXT
        dodoStringArray arr = library::symbols("./module");
        dodoStringArray::iterator i = arr.begin(), j = arr.end();
        for (; i != j; ++i)
            cout << *i << endl;
        cout << endl;
#endif

        library sll("./module");


        ((print)sll["print"])("operator[]");

        ((print)sll.function("print"))("get");
#endif
    } dodo_catch (exception::basic *e)   {
        cout << (dodo::string)*e << "\t" << e->line << endl;
    }
    return 0;
}
