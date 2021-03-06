/**
 * vim indentation settings
 * set tabstop=4
 * set dataiftwidmanager=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;
using namespace dodo::pc;

using namespace std;

sync::thread protector;
sync::data::object data(protector);

int
thread(void *data)
{
    dodo_try {
        cout << endl << (char *)data << ": " << tools::time::now() << endl;
        cout.flush();

        int timeout = *(int *)::data.acquire();
        tools::os::sleep(timeout);
        ::data.release();

        cout << endl << (char *)data << ": " << tools::time::now() << endl;
        cout.flush();
    } dodo_catch (dodo::exception::basic *e)   {
        cout << (dodo::string)*e << "\t" << e->line << "\t" << e->file << endl;
    }

    // throwing exception
    tools::os::os::setWorkingDir("./dir/");

    return 0;
}

int
main(int  argc UNUSED,
     char **argv UNUSED)
{
    dodo_try {
        int *data = new int(1);

        ::data.set((void *)data);

        execution::thread(thread, (void *)"detached", execution::ON_DESTRUCTION_KEEP_ALIVE, true).run();

        execution::manager manager;

        const int amount = 10;

        int pos[amount];
        dodo::string ids[amount];
        for (int i = 0; i < amount; ++i) {
            ids[i] = tools::string::lToString(i);
            pos[i] = manager.add(execution::thread(thread, (void *)ids[i].data(), execution::ON_DESTRUCTION_STOP));
        }

        cout << "Launching threads" << endl;
        cout << tools::time::now() << endl;
        cout.flush();

        for (int i = 0; i < amount; ++i)
            manager.run(pos[i]);

        manager.wait();

        for (int i = 0; i < amount; ++i) {
            dodo::exception::basic *ex = dynamic_cast<execution::thread *>(manager.job(pos[i]))->exception();
            if (ex) {
                cout << "Thread " << i << ":\t" << (dodo::string)*ex << "\t" << ex->line << "\t" << ex->file << endl;
            }
        }

        delete data;
    } dodo_catch (dodo::exception::basic *e)   {
        cout << (dodo::string)*e << "\t" << e->line << "\t" << e->file << endl;
    }

    return 0;
}
