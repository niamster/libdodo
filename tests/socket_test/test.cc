#include <flushSocket.h>
#include <flushSocketTools.h>
#include <systemThreads.h>
#include <systemThreadShares.h>

using namespace dodo;

using namespace std;

systemThreadShares sh;
#define EXIT_POS 1

void *
process(void *data)
{
	flushSocketExchange *fse = (flushSocketExchange *)data;
	
/*	int *pos;
	sh.lock(1,(void *)pos);
	cout << "!!!" << *pos << "!!!\n";
	sh.unlock(1);*/
	
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
	fse->setInBufferSize(4);
	fse->setOutBufferSize(4);
	
	fse->outSize = 7;
	fse->sendStreamString("dasdasd");
	
	std::string rec = "";
	try
	{
		fse->receiveStreamString(rec);
		cout << rec << rec.size() << endl;
		cout.flush();
		if (rec.compare("exit")==0)
		{
			bool *exit_st;
			exit_st = (bool *)sh.lock(EXIT_POS);
			*exit_st = true;
			sh.unlock(EXIT_POS);
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
}

int main(int argc, char **argv)
{
	try
	{	
		stringArr ifaces = flushSocketTools::getInterfacesNames();
		for (int i(0);i<ifaces.size();i++)
		{
			try
			{
				cout << ifaces[i] << ":\t" << flushSocketTools::getInterfaceInfo(ifaces[i]).hwaddr << endl;	
			}
			catch (baseEx ex)
			{
				cout << ifaces[i] << ":\t" << ex << endl;
			}
		}
		
		cout << flushSocketTools::getInterfaceInfo("lo").hwaddr << endl;
		cout << flushSocketTools::getInterfaceInfo("eth0").address << endl;
			
		flushSocket sock(true,PROTO_FAMILY_IPV4/*PROTO_FAMILY_IPV6*//*PROTO_FAMILY_UNIX_SOCKET*/,TRANSFER_TYPE_STREAM);
		
		
/*		sock.connect("127.0.0.1",21,ex1);
		ex1.close();
		
		sock.connect("127.0.0.1",21,ex1);

		flushSocketExchange ex2;
		flushSocketExchange ex2 = ex1;
		sock.connect("127.0.0.1",21,ex2);
*/
		__connInfo info;
		__initialAccept fake;
		sock.setSockOption(SOCKET_REUSE_ADDRESS,true);
		sock.setLingerSockOption(SOCKET_HARD_CLOSE);	
		sock.blockInherited = false;
		
		sock.block(false);
				
		sock.bindNListen("127.0.0.1",7778,3);
		//sock.bindNListen("::",7777);
		//sock.bindNListen("./sock",10,true);
		
		flushSocketExchange conn;

		bool exit_st(false);

		systemThreads th;
		std::vector<int> positions;
		
		sh.add((void *)&exit_st);
		
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
				th.setExecutionLimit(positions.back());
				
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
		
		//flushSocketTools::setLocalName("BUBU");
		
		/*cout << flushSocketTools::getLocalName() << endl;
		cout << flushSocketTools::getHostInfo("192.168.0.1").addresses[0] << endl;
		
		flushSocketTools *pointer[10];
		pointer[0] = &sock;
		cout << pointer[0]->getHostInfo("elessar.mu").addresses[0] << endl;*/
		
	}
	catch(baseEx ex)
	{
		cout << ex << "\t" << ex.line << endl;
		cout.flush();
	}
	
	return 0;
}
