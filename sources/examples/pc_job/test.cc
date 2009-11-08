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

        tools::os::sleep(2);

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

        execution::job *jobs[amount];

        dodoString ids[amount];
        for (int i = 0; i < amount; ++i) {
            ids[i] = tools::string::lToString(i);
            if (i % 2 == 0)
                jobs[i] = new execution::thread(::job1, (void *)ids[i].c_str(), execution::ON_DESTRUCTION_STOP);
            else
                jobs[i] = new execution::process(::job1, (void *)ids[i].c_str(), execution::ON_DESTRUCTION_STOP);
        }

        cout << "Launching jobs" << endl;
        cout << tools::time::millinow() << endl;

        for (int i = 0; i < amount; ++i)
            jobs[i]->run();

        cout << tools::time::millinow() << endl;
        tools::os::sleep(2);
        cout << tools::time::millinow() << endl;
        scheduler.remove(pID);
        pID = scheduler.schedule(&periodic, 100, true);
        cout << tools::time::millinow() << endl;
        tools::os::sleep(2);
        scheduler.remove(pID);
        cout << tools::time::millinow() << endl;
        tools::os::sleep(2);
        cout << tools::time::millinow() << endl;

        for (int i = 0; i < amount; ++i)
            cout << "status: " << jobs[i]->wait() << endl;

        for (int i = 0; i < amount; ++i)
            delete jobs[i];
    } catch (dodo::exception::basic &ex)   {
        cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
    }

    return 0;
}
