/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/exceptionBasic.h>
#include <libdodo/pcThreadCollection.h>
#include <libdodo/pcSyncThreadDataSingle.h>
#include <libdodo/pcSyncThreadDataCollection.h>
#include <libdodo/toolsOs.h>
#include <libdodo/toolsTime.h>
#include <libdodo/toolsMisc.h>

#include <iostream>

using namespace dodo;
using namespace dodo::pc::thread;
using namespace dodo::pc::sync::thread;

using namespace std;

data::single sh;
data::collection shC;

unsigned long shCI;

void *
thread(void *data)
{
	try
	{
		cout << (char *)shC.get(shCI);
		cout << endl << (char *)data << ": " << tools::time::now() << endl;
		cout.flush();

		int timeout = *(int *)sh.acquire();
		tools::os::sleep(timeout);
		sh.release();

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
	shCI = shC.add((char *)"@test@\n");
	try
	{
		int *shared = new int (1);

		sh.set((void *)shared);

#ifdef PTHREAD_EXT
		collection th;

		const int amount = 10;

		int pos[amount];
		dodoString ids[amount];
		for (int i = 0; i < amount; ++i)
		{
			ids[i] = tools::string::lToString(i);
			pos[i] = th.add(thread, (void *)ids[i].c_str());
		}

		for (int i = 0; i < amount; ++i)
			th.run(pos[i]);

		cout << endl << endl << "STARTED" << endl;
		cout << tools::time::now() << endl;
		cout.flush();

		th.wait();
#endif

		delete shared;
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << endl;
	}

	return 0;
}
