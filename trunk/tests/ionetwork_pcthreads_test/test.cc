/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/exceptionBasic.h>
#include <libdodo/ioNetwork.h>
#include <libdodo/toolsNetwork.h>
#include <libdodo/pcThreadCollection.h>
#include <libdodo/pcSyncThreadDataSingle.h>

#include <iostream>

using namespace dodo;
using namespace io::network;
using namespace dodo::pc;
using namespace pc::sync::thread;

using namespace std;

data::single sh;

void *
process(void *data)
{
	exchange *fse = (exchange *)data;
	
	try
	{
		if (fse->isBlocked())
		{
			cout << "CHILD BLOCKED\n";
			cout.flush();
		}
		else
		{
			cout << "CHILD UNBLOCKED\n";
			cout.flush();
		}

		if (fse->isAlive())
		{
			cout << "IT'S ALIVE!\n";
			cout.flush();
		}

		fse->inSize = 4;
		fse->setInBufferSize(1);
		fse->setOutBufferSize(1);

		fse->outSize = 7;
		fse->writeString("test");

		dodoString rec = "";
		try
		{
			fse->readString(rec);

			cout << rec << rec.size() << endl;
			cout.flush();
			if (rec == "exit")
			{
				bool *exit_st;
				exit_st = (bool *)sh.acquire();
				*exit_st = true;
				sh.release();
			}
		}
		catch (dodo::exception::basic ex)
		{
			cout << "Smth happened!" << (string)ex << endl;
			cout.flush();
		}
		catch (...)
		{
			cout << "Smth happened!" << endl;
			cout.flush();
		}

		fse->close();

		if (fse->isAlive())
		{
			cout << "IT'S ALIVE?????\n";
			cout.flush();
		}
		else
		{
			cout << "CLOSED!\n";
			cout.flush();
		}
	}
	catch (dodo::exception::basic ex)
	{
		cout << (string)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	delete fse;

	return NULL;
}

int main(int argc, char **argv)
{
	try
	{
		server sock(CONNECTION_PROTO_FAMILY_IPV4, CONNECTION_TRANSFER_TYPE_STREAM);

		__peerInfo info;
		__initialAccept fake;

		sock.bindNListen("127.0.0.1", 7778, 3);
		sock.setLingerOption(CONNECTION_LINGEROPTION_HARD_CLOSE);
		sock.blockInherited = false;
		sock.block(false);

		exchange conn;

		bool exit_st(false);

#ifdef PTHREAD_EXT
		thread::collection th;
		vector<int> positions;
#endif

		sh.set((void *)&exit_st);

		while (!exit_st)
		{
#ifdef PTHREAD_EXT
			th.sweepTrash();
#endif

			if (sock.accept(fake, info))
			{
				if (sock.isBlocked())
				{
					cout << "PARENT BLOCKED\n";
					cout.flush();
				}

				conn.init(fake);
#ifdef PTHREAD_EXT
				positions.push_back(th.add(process, (void *)(new exchange(conn))));
				th.run(positions.back());
				th.setExecutionLimit(positions.back(), 1);
#else
				process((void *)(new exchange(conn)));
#endif
				conn.close();

#ifdef PTHREAD_EXT
				try
				{
					if (th.isRunning(1))
					{
						cout << "Running!\n";
						cout.flush();
					}
				}
				catch (...)
				{
				}
#endif
			}
		}

#ifdef PTHREAD_EXT
		th.wait();
#endif

	}
	catch (dodo::exception::basic ex)
	{
		cout << (string)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}
