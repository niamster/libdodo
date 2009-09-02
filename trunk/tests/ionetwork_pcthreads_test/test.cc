/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidmanager=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;
using namespace io::network;
using namespace dodo::pc;
using namespace pc::sync::thread;

using namespace std;

pc::sync::thread::data::single data;

int
process(void *data)
{
	exchange *ex = (exchange *)data;

	try
	{
		if (ex->isBlocked())
		{
			cout << "Blocked\n";
			cout.flush();
		}
		else
		{
			cout << "Non blocked\n";
			cout.flush();
		}

		if (ex->isAlive())
		{
			cout << "Alive\n";
			cout.flush();
		}

		ex->blockSize = 4;
		ex->setInBufferSize(1);
		ex->setOutBufferSize(1);

		ex->blockSize = 7;
		ex->writeString("test\n");

		dodoString str = "";
		try
		{
			str = ex->read();

			cout << str << ":" << str.size() << endl;
			cout.flush();
			if (str == "exit")
			{
				bool *exit_condition;
				exit_condition = (bool *)::data.acquire();
				*exit_condition = true;
				::data.release();
			}
		}
		catch (dodo::exception::basic ex)
		{
			cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
			cout.flush();
		}

		ex->close();

		if (ex->isAlive())
		{
			cout << "It's alive\n";
			cout.flush();
		}
		else
		{
			cout << "Closed\n";
			cout.flush();
		}
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	delete ex;

	return 0;
}

int main(int argc, char **argv)
{
	try
	{
		server s(connection::PROTOCOL_FAMILY_IPV4, connection::TRANSFER_STREAM);

		connection::__peer__ info;
		exchange::__init__ accepted;

		s.serve("127.0.0.1", 7778, 3);
		s.setLingerOption(connection::LINGER_OPTION_HARD_CLOSE);
		s.blockInherited = false;
		s.block(false);

		bool exit_condition(false);

		thread::manager manager;
		vector<int> threads;

		::data.set((void *)&exit_condition);

		while (!exit_condition)
		{
			if (s.accept(accepted, info))
			{
				if (s.isBlocked())
				{
					cout << "Blocked\n";
					cout.flush();
				}

				exchange *ex = new exchange(accepted);
				threads.push_back(manager.add(::process, (void *)ex, pc::job::ON_DESTRUCTION_KEEP_ALIVE));
				manager.run(threads.back());

				try
				{
					vector<int>::iterator i = threads.begin(), j = threads.end();
					for (;i!=j;++i)
						if (manager.isRunning(*i))
						{
							cout << *i << " is running\n";
							cout.flush();
						}
						else
						{
							cout << *i << " is finished\n";
							manager.remove(*i);
							cout.flush();
						}
				}
				catch (...)
				{
				}
			}
		}

		manager.wait();
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}
