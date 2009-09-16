/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;
using namespace dodo::pc::job;

using namespace std;

int
job(void *data)
{
	try
	{
		cout << endl << (char *)data << ": " << tools::time::now() << endl;
		cout.flush();

		tools::os::sleep(2);

		cout << endl << (char *)data << ": " << tools::time::now() << endl;
		cout.flush();
	}
	catch (dodo::exception::basic &ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 10;
}

int main(int argc, char **argv)
{
	try
	{
		const int amount = 10;

		dodo::pc::job::manager *manager[amount];

		int jobs[amount];
		dodoString ids[amount];
		for (int i = 0; i < amount; ++i)
		{
			if (i % 2 == 0)
				manager[i] = new thread::manager;
			else
				manager[i] = new process::manager;

			ids[i] = tools::string::lToString(i);
			jobs[i] = manager[i]->add(::job, (void *)ids[i].c_str(), pc::job::ON_DESTRUCTION_STOP);
		}

		cout << "Launching jobs" << endl;
		cout << tools::time::now() << endl;
		cout.flush();

		for (int i = 0; i < amount; ++i)
			manager[i]->run(jobs[i]);

		for (int i = 0; i < amount; ++i)
			cout << "status: " << manager[i]->wait(jobs[i]) << endl;

		for (int i = 0; i < amount; ++i)
			delete manager[i];
	}
	catch (dodo::exception::basic &ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}
