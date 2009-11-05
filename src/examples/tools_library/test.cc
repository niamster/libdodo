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

typedef void (*print)(const dodoString &data);
#endif

int
main(int  argc,
     char **argv)
{
    try {
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
    } catch (dodo::exception::basic &ex)   {
        cout << (dodoString)ex << "\t" << ex.line << endl;
    }
    return 0;
}
