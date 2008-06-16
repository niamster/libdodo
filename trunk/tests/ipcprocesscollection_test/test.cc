#include <libdodo/baseEx.h>
#include <libdodo/ipcProcessCollection.h>
#include <libdodo/toolsOs.h>
#include <libdodo/toolsTime.h>
#include <libdodo/toolsMisc.h>
#include <libdodo/ipcSharedData.h>
#include <libdodo/ipcProcessSharedDataGuard.h>
#include <libdodo/ipcProcessSharedDataCollectionGuard.h>

#include <iostream>

using namespace dodo;
using namespace dodo::ipc;
using namespace process;
using namespace process::shared;

using namespace std;

dataGuard dg;
dataCollectionGuard dgC;
unsigned long dgCI;

void *
process(void *ud)
{
	try
	{
		dodo::ipc::shared::data shD("dodo");
		char *dt = (char *)shD.map(100);
		cout << dt << endl;

		cout << (char *)dgC.get(dgCI);
		cout << (char *)dg.acquire();
		dg.release();

		cout << endl << (char *)dt << ": " << tools::time::now() << endl;
		cout.flush();

		tools::os::sleep(10);

		cout << endl << (char *)dt << ": " << tools::time::now() << endl;
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
		dodo::ipc::shared::data shD("dodo");
		char *data = (char *)shD.map(100);
		strcpy(data, "test");

		dgCI = dgC.add((char *)"@test@\n");
		dg.set((char *)"!test!\n");

		collection pr;

		const int amount = 10;

		int pos[amount];
		dodoString ids[amount];
		for (int i = 0; i < amount; ++i)
		{
			ids[i] = tools::string::lToString(i);
			pos[i] = pr.add(::process, (void *)ids[i].c_str());
		}

		for (int i = 0; i < amount; ++i)
			pr.run(pos[i]);

		cout << endl << endl << "STARTED" << endl;
		cout << tools::time::now() << endl;
		cout.flush();

		pr.wait();
	}
	catch (baseEx ex)
	{
		cout << (string)ex << "\t" <<  ex.file << "\t" << ex.line << endl;
	}

	return 0;
}
