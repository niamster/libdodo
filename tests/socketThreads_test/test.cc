#include <baseEx.h>
#include <flushSocket.h>
#include <flushSocketTools.h>
#include <systemThreads.h>
#include <systemThreadShares.h>

using namespace dodo;

using namespace std;

systemThreadShares sh;

void *
process(void *data)
{
	flushSocketExchange *fse = (flushSocketExchange *)data;
	
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
	//fse->sendStreamString("dasdasd");
	fse->sendString("dasdasd");
	
	std::string rec = "";
	try
	{
		//fse->receiveStreamString(rec);
		fse->receiveString(rec);
		
		cout << rec << rec.size() << endl;
		cout.flush();
		if (rec.compare("exit")==0)
		{
			bool *exit_st;
			exit_st = (bool *)sh.lock();
			*exit_st = true;
			sh.unlock();
		}
	}
	catch (baseEx ex)
	{
		cout << "Smth happened!" << ex << endl;
		cout.flush();
	}
	catch(...)
	{
		cout << "Smth happened!" << endl;
		cout.flush();		
	}
	
	flushSocketExchange::deleteCopy(fse);
	
	return NULL;
}

int main(int argc, char **argv)
{
	try
	{	
		flushSocket sock(true,PROTO_FAMILY_IPV4/*PROTO_FAMILY_IPV6*//*PROTO_FAMILY_UNIX_SOCKET*/,TRANSFER_TYPE_STREAM);
		
		__connInfo info;
		__initialAccept fake;
				
		sock.bindNListen("127.0.0.1",7778,3);
		sock.setLingerSockOption(SOCKET_HARD_CLOSE);	
		sock.blockInherited = false;
		sock.block(false);
		//sock.bindNListen("::",7777);
		//sock.bindNListen("./sock",10,true);
		
		flushSocketExchange conn;

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
					cout << ex << "\t" << ex.line << endl;
					cout.flush();
				}
			}
		}
		
		th.wait();
		
	}
	catch(baseEx ex)
	{
		cout << ex << "\t" << ex.line << endl;
		cout.flush();
	}
	
	return 0;
}
