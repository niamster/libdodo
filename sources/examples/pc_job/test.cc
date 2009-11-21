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
    try {
        cout << endl << (char *)data << ": " << tools::time::millinow() << endl;
    } catch (dodo::exception::basic &ex)   {
        cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
    }

    return 10;
}

int
job1(void *data)
{
    try {
        cout << endl << ">>" << (char *)data << ": " << tools::time::millinow() << endl;

        tools::os::sleep(tools::string::stringToUL((char *)data));

        cout << endl << "<<" << (char *)data << ": " << tools::time::millinow() << endl;
    } catch (dodo::exception::basic &ex)   {
        cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
    }

    return 10;
}

int
main(int  argc UNUSED,
     char **argv UNUSED)
{
    try {
        const int amount = 10;

        execution::thread periodic(::job0, (void *)"periodic", execution::ON_DESTRUCTION_STOP);
        execution::thread oneshot0(::job0, (void *)"oneshot", execution::ON_DESTRUCTION_STOP);
        execution::process oneshot1(::job0, (void *)"oneshot", execution::ON_DESTRUCTION_STOP);
        execution::scheduler scheduler;

        unsigned long pID = scheduler.schedule(&periodic, 1000, true);
        scheduler.schedule(&oneshot0, 2000);
        scheduler.schedule(&oneshot1, 3000);

        execution::manager manager;

        dodoString ids[amount];
        int pos[amount];
        for (int i = 0; i < amount; ++i) {
            ids[i] = tools::string::lToString(i);
            if (i % 2 == 0) {
                pos[i] = manager.add(execution::thread(::job1, (void *)ids[i].c_str(), execution::ON_DESTRUCTION_STOP));
                cout << "Job #"<< pos[i] << " is a thread" << endl;
            } else {
                pos[i] = manager.add(execution::process(::job1, (void *)ids[i].c_str(), execution::ON_DESTRUCTION_STOP));
                cout << "Job #"<< pos[i] << " is a process" << endl;
            }
        }

        cout << "Launching jobs" << endl;
        cout << tools::time::millinow() << endl;

        for (int i = 0; i < amount; ++i)
            manager.run(pos[i]);

        for (int i = 0; i < amount; ++i)
            cout << "Job #"<< pos[i] << " is " << (manager.isRunning(pos[i])?"running":"not running") << endl;

        cout << tools::time::millinow() << endl;
        tools::os::sleep(2);
        cout << tools::time::millinow() << endl;
        scheduler.remove(pID);

        for (int i = 0; i < amount; ++i)
            cout << "Job #"<< pos[i] << " is " << (manager.isRunning(pos[i])?"running":"not running") << endl;

        pID = scheduler.schedule(&periodic, 100, true);
        cout << tools::time::millinow() << endl;
        tools::os::sleep(2);
        scheduler.remove(pID);
        cout << tools::time::millinow() << endl;
        tools::os::sleep(2);
        cout << tools::time::millinow() << endl;

        for (int i = 0; i < amount; ++i)
            cout << "Job #"<< pos[i] << " is " << (manager.isRunning(pos[i])?"running":"not running") << endl;

        for (int i = 0; i < amount; ++i)
            cout << "status: " << manager.wait(pos[i]) << endl;

        for (int i = 0; i < amount; ++i)
            cout << "Job #"<< pos[i] << " is " << (manager.isRunning(pos[i])?"running":"not running") << endl;
    } catch (dodo::exception::basic &ex)   {
        cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
    }

    return 0;
}
