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
	//fse->writeStreamString("dasdasd");
	fse->writeString("dasdasd");
	
	dodoString rec = "";
	try
	{
		//fse->readStreamString(rec);
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
		ioSocket sock(true,IOSOCKETOPTIONS_PROTO_FAMILY_IPV4/*IOSOCKETOPTIONS_PROTO_FAMILY_IPV6*//*IOSOCKETOPTIONS_PROTO_FAMILY_UNIX_SOCKET*/,IOSOCKETOPTIONS_TRANSFER_TYPE_STREAM);
		
		__connInfo info;
		__initialAccept fake;
				
		sock.bindNListen("127.0.0.1",7778,3);
		//sock.bindNListen("::",7777);
		//sock.bindNListen("./sock",10,true);
		sock.setLingerSockOption(IOSOCKETOPTIONS_SOCKET_HARD_CLOSE);	
		sock.blockInherited = false;
		sock.block(false);
		
		ioSocketExchange conn;

		bool exit_st(false);

		systemThreads th;
		std::vector<int> positions;
		
		sh.set((void *)&exit_st);
		
		while(!exit_st)
		{
			th.sweepTrash();
			
			if (sock.accept(fake,info))
			{
				if (sock.isBlocked())
				{
					std::cout << "PARENT BLOCKED\n";
					cout.flush();
				}
					
				conn.init(fake);
				positions.push_back(th.add(process,(void *)conn.createCopy()));
				th.run(positions.back());
				th.setExecutionLimit(positions.back(), 1);
				
				try
				{
					if (th.isRunning(1))
					{
						std::cout << "WOW\n";
						cout.flush();
					}
				}
				catch(baseEx ex)
				{
					cout << (string)ex << "\t" << ex.line << "\t" << ex.file << endl;
					cout.flush();
				}
			}
		}
		
		th.wait();
		
	}
	catch(baseEx ex)
	{
		cout << (string)ex << "\t" << ex.line << "\t" << ex.file << endl;
		cout.flush();
	}
	
	return 0;
}
