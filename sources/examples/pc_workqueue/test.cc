/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;
using namespace dodo::pc;

using namespace std;

sync::thread lock;
unsigned long done = 0;

int
work(void *data)
{
    dodo_try {
        tools::os::sleep(1);

        lock.acquire();
        ++done;
        cout << "#" << done << ": " << (char *)data << " : " << tools::time::nowMs() << endl, cout.flush();
        lock.release();
    } dodo_catch (dodo::exception::basic *e)   {
        cout << (dodo::string)*e << "\t" << e->line << "\t" << e->file << endl;
    }

    return 0;
}

int
main(int  argc UNUSED,
     char **argv UNUSED)
{
    dodo_try {
        execution::workqueue wq(10, 5, 1);

        tools::os::sleep(1);

        cout << "Launching jobs: " << tools::time::nowMs() << endl;

        for (int i=0;i<100;++i)
            wq.add(work, (void *)"work");
        wq.add(work, (void *)"last");

        tools::os::sleep(30);
    } dodo_catch (dodo::exception::basic *e)   {
        cout << (dodo::string)*e << "\t" << e->line << "\t" << e->file << endl;
    }

    return 0;
}
