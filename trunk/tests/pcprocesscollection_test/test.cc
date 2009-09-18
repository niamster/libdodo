/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>
#include <string.h>

using namespace dodo;
using namespace dodo::pc::job::process;

using namespace std;

pc::sync::process::data::single data0(0);
pc::sync::process::data::single data1;
pc::sync::process::section section(0);
pc::sync::process::data::collection collection;

unsigned long collectionIdx;

const int key0 = 100;
const int key1 = 101;

int
process0(void *data)
{
	cout << "process0:" << (char *)data << endl, cout.flush();

	try
	{
		dodo::data::memory::shared shared(key0);
		io::memory shM((char *)shared.data(), shared.size(), io::memory::FLAGS_EXTERN | io::memory::FLAGS_FIXED_LENGTH);
		cout << "Shared memory: " << endl, cout.flush();
		cout << "Data: '" << shM << "'" << endl, cout.flush();

		cout << (char *)collection.get(collectionIdx), cout.flush();

		cout << (char *)data0.acquire(1), cout.flush();
		tools::os::microSleep(1000);
		data0.release();

		cout << endl << shM << ": " << tools::time::now() << endl, cout.flush();

		cout << (char *)data1.acquire(), cout.flush();
		data1.release();

		cout << endl << shM << ": " << tools::time::now() << endl, cout.flush();
	}
	catch (dodo::exception::basic &ex)
	{
		cout << (dodoString)ex << ex.line << endl, cout.flush();
	}

	return 0;
}

int
process1(void *data)
{
	pc::sync::protector pg(&section);

	cout << "process1(protected):" << (char *)data << endl, cout.flush();

	try
	{
		pc::sync::protector pg(&section); /* section could be acquired recursively by current process */

		dodo::data::memory::shared shared(key0);
		unsigned long size = shared.size();
		cout << "Shared data: " << endl, cout.flush();
		cout << "Size: " << size << endl, cout.flush();
		char *dt = (char *)shared.map(size);
		cout << "Data: " << dt << endl, cout.flush();

		cout << (char *)collection.get(collectionIdx), cout.flush();

		cout << (char *)data0.acquire(), cout.flush();
		data0.release();

		cout << endl << (char *)dt << ": " << tools::time::now() << endl, cout.flush();

		tools::os::sleep(1);

		cout << (char *)data1.acquire(), cout.flush();
		data1.release();

		cout << endl << (char *)dt << ": " << tools::time::now() << endl, cout.flush();
	}
	catch (dodo::exception::basic &ex)
	{
		cout << (dodoString)ex << ex.line << endl, cout.flush();
	}

	return 0;
}

int main(int argc, char **argv)
{
	try
	{
		data1.open(key1);
		data1.set((char *)"%test%\n");

		dodo::data::memory::shared shared(key0);
		char *data = (char *)shared.map(100);
		strcpy(data, "test");

		collectionIdx = collection.add((char *)"@test@\n");
		data0.set((char *)"!test!\n");

		manager manager;

		const int amount = 10;

		int processes[amount];
		dodoString ids[amount];
		for (int i = 0; i < amount; ++i)
		{
			ids[i] = tools::string::lToString(i);
			if (i%2 == 0)
				processes[i] = manager.add(::process0, (void *)ids[i].c_str(), pc::job::ON_DESTRUCTION_STOP);
			else
				processes[i] = manager.add(::process1, (void *)ids[i].c_str(), pc::job::ON_DESTRUCTION_STOP);
		}

		for (int i = 0; i < amount; ++i)
			manager.run(processes[i]);

		cout << endl << endl << "STARTED" << endl;
		cout << tools::time::now() << endl;
		cout.flush();

		manager.wait();

		data1.close();
	}
	catch (dodo::exception::basic &ex)
	{
		cout << (dodoString)ex << "\t" <<  ex.file << "\t" << ex.line << endl;
	}

	try
	{
		dodo::data::memory::shared::remove(key0);
		data1.remove(key1);
	}
	catch (dodo::exception::basic &ex)
	{
		cout << (dodoString)ex << "\t" <<  ex.file << "\t" << ex.line << endl;
	}

	return 0;
}
