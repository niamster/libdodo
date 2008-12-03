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
#include <libdodo/dataMemoryShared.h>
#include <libdodo/pcSyncProcessDataSingle.h>
#include <libdodo/pcSyncProcessDataCollection.h>

#include <iostream>

using namespace dodo;
using namespace dodo::pc;
using namespace process;

using namespace std;

pc::sync::process::data::single dg;
pc::sync::process::section sec;
pc::sync::process::data::collection dgC;

unsigned long dgCI;

const int shKey = 100;

int
process(void *ud)
{
	cout << "process:" << (char *)ud << endl, cout.flush();

	try
	{
		dodo::data::memory::shared shD(shKey);
		unsigned long size = shD.getSize();
		cout << "Shared memory: " << endl, cout.flush();
		cout << "Size: " << size << endl, cout.flush();
		char *dt = (char *)shD.map();
		cout << "Data: " << dt << endl, cout.flush();
		
		cout << (char *)dgC.get(dgCI), cout.flush();
		
		cout << (char *)dg.acquire(1), cout.flush();
		tools::os::microSleep(1000);
		dg.release();

		cout << endl << (char *)dt << ": " << tools::time::now() << endl, cout.flush();

		cout << (char *)dg.acquire(), cout.flush();
		dg.release();

		cout << endl << (char *)dt << ": " << tools::time::now() << endl, cout.flush();
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << ex.line << endl, cout.flush();
	}

	return 0;
}

int
processLocked(void *ud)
{
	sec.acquire();
	
	cout << "processLocked:" << (char *)ud << endl, cout.flush();

	try
	{
		dodo::data::memory::shared shD(shKey);
		unsigned long size = shD.getSize();
		cout << "Shared memory: " << endl, cout.flush();
		cout << "Size: " << size << endl, cout.flush();
		char *dt = (char *)shD.map(size);
		cout << "Data: " << dt << endl, cout.flush();
		
		cout << (char *)dgC.get(dgCI), cout.flush();
	
		cout << (char *)dg.acquire(), cout.flush();
		dg.release();

		cout << endl << (char *)dt << ": " << tools::time::now() << endl, cout.flush();

		tools::os::sleep(1);
		
		cout << (char *)dg.acquire();
		dg.release();

		cout << endl << (char *)dt << ": " << tools::time::now() << endl, cout.flush();
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << ex.line << endl, cout.flush();
	}

	sec.release();

	return 0;
}

int main(int argc, char **argv)
{
	try
	{
		dodo::data::memory::shared shD(shKey);
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
			if (i%2 == 0)
				pos[i] = pr.add(::process, (void *)ids[i].c_str());
			else
				pos[i] = pr.add(::processLocked, (void *)ids[i].c_str());
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
		cout << (dodoString)ex << "\t" <<  ex.file << "\t" << ex.line << endl;
	}

	try
	{
		dodo::data::memory::shared::remove(shKey);
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << "\t" <<  ex.file << "\t" << ex.line << endl;
	}

	return 0;
}
