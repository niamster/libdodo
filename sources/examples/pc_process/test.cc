/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>
#include <string.h>

using namespace dodo;
using namespace dodo::pc;

using namespace std;

const int data_key = 100;
const int lock_key = 101;

sync::process protector0(0);
sync::data::object data0(protector0);

sync::process protector1(lock_key);
sync::data::object data1(protector1);

pc::sync::process section(0);

int
process0(void *data)
{
    cout << "process0:" << (char *)data << endl, cout.flush();

    dodo_try {
        dodo::data::memory::shared shared(data_key);
        io::memory memory((char *)shared.data(), shared.size(), io::memory::FLAGS_EXTERN | io::memory::FLAGS_FIXED_LENGTH);
        cout << "Shared memory: " << endl, cout.flush();
        cout << "Data: '" << memory << "'" << endl, cout.flush();

        cout << (char *)data0.acquire(1), cout.flush();
        tools::os::sleepMs(1);
        data0.release();

        cout << endl << memory << ": " << tools::time::now() << endl, cout.flush();

        cout << (char *)data1.acquire(), cout.flush();
        data1.release();

        cout << endl << memory << ": " << tools::time::now() << endl, cout.flush();
    } dodo_catch (exception::basic *e)   {
        cout << (dodo::string)*e << e->line << endl, cout.flush();
    }

    return 0;
}

int
process1(void *data)
{
    sync::stack pg(&section);

    cout << "process1(protected):" << (char *)data << endl, cout.flush();

    dodo_try {
        pc::sync::stack pg(&section); /* section could be acquired recursively by current process */

        dodo::data::memory::shared shared(data_key);
        unsigned long size = shared.size();
        cout << "Shared data: " << endl, cout.flush();
        cout << "Size: " << size << endl, cout.flush();
        char *dt = (char *)shared.map(size);
        cout << "Data: " << dt << endl, cout.flush();

        cout << (char *)data0.acquire(), cout.flush();
        data0.release();

        cout << endl << (char *)dt << ": " << tools::time::now() << endl, cout.flush();

        tools::os::sleep(1);

        cout << (char *)data1.acquire(), cout.flush();
        data1.release();

        cout << endl << (char *)dt << ": " << tools::time::now() << endl, cout.flush();
    } dodo_catch (exception::basic *e)   {
        cout << (dodo::string)*e << e->line << endl, cout.flush();
    }

    return 0;
}

int
main(int  argc UNUSED,
     char **argv UNUSED)
{
	exception::basic *e;
    
	dodo_try {
        data1.set((char *)"%test%\n");

        dodo::data::memory::shared shared(data_key);
        char *data = (char *)shared.map(100);
        strcpy(data, "test");

        data0.set((char *)"!test!\n");

        execution::manager manager;

        const int amount = 10;

        int processes[amount];
        dodo::string ids[amount];
        for (int i = 0; i < amount; ++i) {
            ids[i] = tools::string::lToString(i);
            if (i % 2 == 0)
                processes[i] = manager.add(execution::process(::process0, (void *)ids[i].data(), execution::ON_DESTRUCTION_STOP));
            else
                processes[i] = manager.add(execution::process(::process1, (void *)ids[i].data(), execution::ON_DESTRUCTION_STOP));
        }

        {
            sync::stack pg(&section);

            for (int i = 0; i < amount; ++i)
                manager.run(processes[i]);

            cout << endl << endl << "STARTED" << endl;
            cout << tools::time::now() << endl;
            cout.flush();
        }

        manager.wait();
    } dodo_catch (e)   {
        cout << (dodo::string)*e << "\t" <<  e->file << "\t" << e->line << endl;
    }

    dodo_try {
        data::memory::shared::remove(data_key);
        sync::process::remove(lock_key);
    } dodo_catch (e)   {
        cout << (dodo::string)*e << "\t" <<  e->file << "\t" << e->line << endl;
    }

    return 0;
}
