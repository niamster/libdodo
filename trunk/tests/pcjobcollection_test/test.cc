/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/exceptionBasic.h>
#include <libdodo/pcJobCollection.h>
#include <libdodo/pcProcessCollection.h>
#include <libdodo/pcThreadCollection.h>
#include <libdodo/toolsOs.h>
#include <libdodo/toolsTime.h>
#include <libdodo/toolsMisc.h>

#include <iostream>

using namespace dodo;
using namespace dodo::pc;

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
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << ex.line << endl;
	}

	return NULL;
}

int main(int argc, char **argv)
{
	try
	{
		const int amount = 10;

		dodo::pc::job::collection *pr[amount];

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
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << endl;
	}

	return 0;
}
