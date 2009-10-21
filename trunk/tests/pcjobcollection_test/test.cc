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
job(void *data)
{
    try {
        cout << endl << (char *)data << ": " << tools::time::now() << endl;
        cout.flush();

        tools::os::sleep(2);

        cout << endl << (char *)data << ": " << tools::time::now() << endl;
        cout.flush();
    } catch (dodo::exception::basic &ex)   {
        cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
    }

    return 10;
}

int
main(int  argc,
     char **argv)
{
    try {
        const int amount = 10;

        execution::job *jobs[amount];

        dodoString ids[amount];
        for (int i = 0; i < amount; ++i) {
            ids[i] = tools::string::lToString(i);
            if (i % 2 == 0)
                jobs[i] = new execution::thread(::job, (void *)ids[i].c_str(), execution::ON_DESTRUCTION_STOP);
            else
                jobs[i] = new execution::process(::job, (void *)ids[i].c_str(), execution::ON_DESTRUCTION_STOP);
        }

        cout << "Launching jobs" << endl;
        cout << tools::time::now() << endl;
        cout.flush();

        for (int i = 0; i < amount; ++i)
            jobs[i]->run();

        for (int i = 0; i < amount; ++i)
            cout << "status: " << jobs[i]->wait() << endl;

        for (int i = 0; i < amount; ++i)
            delete jobs[i];
    } catch (dodo::exception::basic &ex)   {
        cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
    }

    return 0;
}
