#include <libdodo/baseEx.h>
#include <libdodo/ipcJobCollection.h>
#include <libdodo/ipcProcessCollection.h>
#include <libdodo/ipcThreadCollection.h>
#include <libdodo/toolsOs.h>
#include <libdodo/toolsTime.h>
#include <libdodo/toolsMisc.h>

#include <iostream>

using namespace dodo;
using namespace dodo::ipc;

using namespace std;

void *
job(void *data)
{
	try
	{
		cout << endl << (char *)data << ": " << tools::time::now() << endl;
		cout.flush();

		tools::os::sleep(10);

		cout << endl << (char *)data << ": " << tools::time::now() << endl;
		cout.flush();
	}
	catch (baseEx ex)
	{
		cout << (string)ex << ex.line << endl;
	}

	return NULL;
}

int main(int argc, char **argv)
{
	try
	{
		const int amount = 10;

		dodo::ipc::job::collection *pr[amount];

		int pos[amount];
		dodoString ids[amount];
		for (int i = 0; i < amount; ++i)
		{
			if (i % 2 == 0)
				pr[i] = new thread::collection;
#ifdef PTHREAD_EXT
			else
				pr[i] = new process::collection;
#endif

			ids[i] = tools::string::lToString(i);
			pos[i] = pr[i]->add(::job, (void *)ids[i].c_str());
		}

		for (int i = 0; i < amount; ++i)
			pr[i]->run(pos[i]);

		cout << endl << endl << "STARTED" << endl;
		cout << tools::time::now() << endl;
		cout.flush();

		for (int i = 0; i < amount; ++i)
			pr[i]->wait();

		for (int i = 0; i < amount; ++i)
			delete pr[i];
	}
	catch (baseEx ex)
	{
		cout << (string)ex << endl;
	}

	return 0;
}
