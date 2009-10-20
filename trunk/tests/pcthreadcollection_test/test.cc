/**
 * vim indentation settings
 * set tabstop=4
 * set dataiftwidmanager=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;
using namespace dodo::pc;

using namespace std;

sync::thread protector;
sync::data::object data(protector);

int
thread(void *data)
{
	try
	{
		cout << endl << (char *)data << ": " << tools::time::now() << endl;
		cout.flush();

		int timeout = *(int *)::data.acquire();
		tools::os::sleep(timeout);
		::data.release();

		cout << endl << (char *)data << ": " << tools::time::now() << endl;
		cout.flush();
	}
	catch (dodo::exception::basic &ex)
	{
		cout << (dodoString)ex << ex.line << endl;
	}

	return 0;
}

int main(int argc, char **argv)
{
	try
	{
		int *data = new int(1);

		::data.set((void *)data);

		execution::manager<execution::thread> manager;

		const int amount = 10;

		int pos[amount];
		dodoString ids[amount];
		for (int i = 0; i < amount; ++i)
		{
			ids[i] = tools::string::lToString(i);
			pos[i] = manager.add(execution::thread(thread, (void *)ids[i].c_str(), execution::ON_DESTRUCTION_STOP));
		}

		cout << "Launching threads" << endl;
		cout << tools::time::now() << endl;
		cout.flush();

		for (int i = 0; i < amount; ++i)
			manager.run(pos[i]);

		manager.wait();

		delete data;
	}
	catch (dodo::exception::basic &ex)
	{
		cout << (dodoString)ex << endl;
	}

	return 0;
}
