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

int
job0(void *data)
{
    dodo_try {
        cout << endl << (char *)data << ": " << tools::time::nowMs() << endl;
    } dodo_catch (dodo::exception::basic *e)   {
        cout << (dodo::string)*e << "\t" << e->line << "\t" << e->file << endl;
    }

    return 10;
}

int
job1(void *data)
{
    dodo_try {
        cout << endl << ">>" << (char *)data << ": " << tools::time::nowMs() << endl;

        tools::os::sleep(tools::string::stringToUL((char *)data));

        cout << endl << "<<" << (char *)data << ": " << tools::time::nowMs() << endl;
    } dodo_catch (dodo::exception::basic *e)   {
        cout << (dodo::string)*e << "\t" << e->line << "\t" << e->file << endl;
    }

    return 10;
}

int
main(int  argc UNUSED,
     char **argv UNUSED)
{
    dodo_try {
        const int amount = 10;

        execution::scheduler scheduler;

        unsigned long pID = scheduler.schedule(execution::thread(::job0, (void *)"periodic", execution::ON_DESTRUCTION_STOP), 1000, true);
        scheduler.schedule(execution::thread(::job0, (void *)"oneshot thread", execution::ON_DESTRUCTION_STOP), 2000);
        scheduler.schedule(execution::process(::job0, (void *)"oneshot process", execution::ON_DESTRUCTION_STOP), 3000);

        execution::manager manager;

        dodo::string ids[amount];
        int pos[amount];
        for (int i = 0; i < amount; ++i) {
            ids[i] = tools::string::lToString(i);
            if (i % 2 == 0) {
                pos[i] = manager.add(execution::thread(::job1, (void *)ids[i].data(), execution::ON_DESTRUCTION_STOP));
                cout << "Job #"<< pos[i] << " is a thread" << endl;
            } else {
                pos[i] = manager.add(execution::process(::job1, (void *)ids[i].data(), execution::ON_DESTRUCTION_STOP));
                cout << "Job #"<< pos[i] << " is a process" << endl;
            }
        }

        cout << "Launching jobs" << endl;
        cout << tools::time::nowMs() << endl;

        for (int i = 0; i < amount; ++i)
            manager.run(pos[i]);

        for (int i = 0; i < amount; ++i)
            cout << "Job #"<< pos[i] << " is " << (manager.isRunning(pos[i])?"running":"not running") << endl;

        cout << tools::time::nowMs() << endl;
        tools::os::sleep(2);
        cout << tools::time::nowMs() << endl;
        scheduler.remove(pID);

        for (int i = 0; i < amount; ++i)
            cout << "Job #"<< pos[i] << " is " << (manager.isRunning(pos[i])?"running":"not running") << endl;

        pID = scheduler.schedule(execution::thread(::job0, (void *)"periodic", execution::ON_DESTRUCTION_STOP), 100, true);
        cout << tools::time::nowMs() << endl;
        tools::os::sleep(2);
        scheduler.remove(pID);
        cout << tools::time::nowMs() << endl;
        tools::os::sleep(2);
        cout << tools::time::nowMs() << endl;

        for (int i = 0; i < amount; ++i)
            cout << "Job #"<< pos[i] << " is " << (manager.isRunning(pos[i])?"running":"not running") << endl;

        for (int i = 0; i < amount; ++i)
            cout << "status: " << manager.wait(pos[i]) << endl;

        for (int i = 0; i < amount; ++i)
            cout << "Job #"<< pos[i] << " is " << (manager.isRunning(pos[i])?"running":"not running") << endl;
    } dodo_catch (dodo::exception::basic *e)   {
        cout << (dodo::string)*e << "\t" << e->line << "\t" << e->file << endl;
    }

    return 0;
}
