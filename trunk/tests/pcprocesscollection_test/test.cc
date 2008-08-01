/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/exceptionBasic.h>
#include <libdodo/pcProcessCollection.h>
#include <libdodo/toolsOs.h>
#include <libdodo/toolsTime.h>
#include <libdodo/toolsMisc.h>
#include <libdodo/dataShared.h>
#include <libdodo/pcSyncProcessDataSingle.h>
#include <libdodo/pcSyncProcessDataCollection.h>

#include <iostream>

using namespace dodo;
using namespace dodo::pc;
using namespace process;

using namespace std;

pc::sync::process::data::single dg;
pc::sync::process::data::collection dgC;

unsigned long dgCI;

void *
process(void *ud)
{
	try
	{
		dodo::data::shared shD("dodo");
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
	catch (dodo::exception::basic ex)
	{
		cout << (string)ex << ex.line << endl;
	}

	return NULL;
}

int main(int argc, char **argv)
{
	try
	{
		dodo::data::shared shD("dodo");
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
	catch (dodo::exception::basic ex)
	{
		cout << (string)ex << "\t" <<  ex.file << "\t" << ex.line << endl;
	}

	return 0;
}
