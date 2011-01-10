/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;

using namespace std;

int
main(int  argc UNUSED,
     char **argv UNUSED)
{
    dodo_try {
        cout << tools::time::byFormat("%A-%Y-%H", tools::time::now()) << endl;

        cout << tools::time::now() << endl;

        tools::os::sleep(3);

        cout << tools::time::now() << endl;

        cout << tools::time::week(tools::time::now(), "%A")[0] << endl;

        cout << tools::time::timestamp(tools::time::now()).sec << endl;
    } dodo_catch (dodo::exception::basic *e)   {
        cout << (dodo::string)*e << "\t" << e->line << endl;
    }

    return 0;
}
