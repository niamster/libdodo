#include <libdodo/baseEx.h>
#include <libdodo/ioSocket.h>
#include <libdodo/ioSocketTools.h>
#include <libdodo/systemThreads.h>
#include <libdodo/systemThreadSharedDataGuard.h>

#include <iostream>

using namespace dodo;

using namespace std;

systemThreadSharedDataGuard sh;

void *
process(void *data)
{
	ioSocketExchange *fse = (ioSocketExchange *)data;
	
	if (fse->isBlocked())
	{
		std::cout << "CHILD BLOCKED\n";
		cout.flush();
	}
	else
	{
		std::cout << "CHILD UNBLOCKED\n";
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
			exit_st = (bool *)sh.lock();
			*exit_st = true;
			sh.unlock();
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
	
	ioSocketExchange::deleteCopy(fse);
	
	return NULL;
}

int main(int argc, char **argv)
{
	try
	{	
		ioSocket sock(true,IOSOCKETOPTIONS_PROTO_FAMILY_IPV4,IOSOCKETOPTIONS_TRANSFER_TYPE_STREAM);
		
		__connInfo info;
		__initialAccept fake;
				
		sock.bindNListen("127.0.0.1",7778,3);
		sock.setLingerSockOption(IOSOCKETOPTIONS_SOCKET_HARD_CLOSE);	
		sock.blockInherited = false;
		sock.block(false);
		
		ioSocketExchange conn;

		bool exit_st(false);

#ifdef PTHREAD_EXT
		systemThreads th;
		std::vector<int> positions;
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
					std::cout << "PARENT BLOCKED\n";
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
						std::cout << "Running!\n";
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