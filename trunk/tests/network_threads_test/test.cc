#include <libdodo/baseEx.h>
#include <libdodo/ioNetwork.h>
#include <libdodo/toolsNetwork.h>
#include <libdodo/systemThreadCollection.h>
#include <libdodo/systemThreadSharedDataGuard.h>

#include <iostream>

using namespace dodo;
using namespace io::network;
using namespace dodo::system;

using namespace std;

thread::shared::dataGuard sh;

void *
process(void *data)
{
	exchange *fse = (exchange *)data;
	
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
	catch (baseEx ex)
	{
		cout << "Smth happened!" << (string)ex << endl;
		cout.flush();
	}
	catch(...)
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

	exchange::deleteCopy(fse);
	
	return NULL;
}

int main(int argc, char **argv)
{
	try
	{	
		server sock(OPTIONS_PROTO_FAMILY_IPV4,OPTIONS_TRANSFER_TYPE_STREAM);
		
		__connInfo info;
		__initialAccept fake;
				
		sock.bindNListen("127.0.0.1",7778,3);
		sock.setLingerOption(OPTIONS_LINGEROPTION_HARD_CLOSE);	
		sock.blockInherited = false;
		sock.block(false);
		
		exchange conn;

		bool exit_st(false);

#ifdef PTHREAD_EXT
		thread::collection th;
		vector<int> positions;
#endif
		
		sh.set((void *)&exit_st);
		
		while(!exit_st)
		{
#ifdef PTHREAD_EXT
			th.sweepTrash();
#endif
			
			if (sock.accept(fake,info))
			{
				if (sock.isBlocked())
				{
					cout << "PARENT BLOCKED\n";
					cout.flush();
				}
					
				conn.init(fake);
#ifdef PTHREAD_EXT
				positions.push_back(th.add(process,(void *)conn.createCopy()));
				th.run(positions.back());
				th.setExecutionLimit(positions.back(), 1);
#else
				process((void *)conn.createCopy());
#endif
				
#ifdef PTHREAD_EXT
				try
				{
					if (th.isRunning(1))
					{
						cout << "Running!\n";
						cout.flush();
					}
				}
				catch(baseEx ex)
				{
					cout << (string)ex << "\t" << ex.line << "\t" << ex.file << endl;
					cout.flush();
				}
#endif
			}
		}
		
#ifdef PTHREAD_EXT
		th.wait();
#endif
		
	}
	catch(baseEx ex)
	{
		cout << (string)ex << "\t" << ex.line << "\t" << ex.file << endl;
		cout.flush();
	}
	
	return 0;
}
