#include <baseEx.h>
#include <flushSocket.h>
#include <flushSocketTools.h>

using namespace dodo;

using namespace std;

void
process(flushSocketExchange fse)
{
	
	if (fse.isBlocked())
	{
		std::cout << "CHILD BLOCKED\n";
		cout.flush();
	}
	else
	{
		std::cout << "CHILD UNBLOCKED\n";
		cout.flush();
	}
	
	fse.inSize = 4;
	fse.setInBufferSize(1);
	fse.setOutBufferSize(1);
	
	fse.outSize = 7;
	//fse.sendStreamString("dasdasd");
	fse.sendString("dasdasd");
	
	std::string rec = "";
	try
	{
		//fse.receiveStreamString(rec);
		fse.receiveString(rec);
		
		cout << rec << rec.size() << endl;
		cout.flush();
		if (rec.compare("exit")==0)
		{
			exit(0);
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
}

int main(int argc, char **argv)
{
	try
	{	
		dodoStringArr ifaces = flushSocketTools::getInterfacesNames();
		for (unsigned int i(0);i<ifaces.size();i++)
		{
			try
			{
				cout << ifaces[i] << ":\t" << flushSocketTools::getInterfaceInfo(ifaces[i]).hwaddr << endl;	
				cout << ifaces[i] << ":\t" << flushSocketTools::getInterfaceInfo(ifaces[i]).broadcast << endl;	
				cout << ifaces[i] << ":\t" << flushSocketTools::getInterfaceInfo(ifaces[i]).netmask << endl;	
			}
			catch (baseEx ex)
			{
				cout << ifaces[i] << ":\t" << ex << endl;
			}
		}
		
		cout << flushSocketTools::getInterfaceInfo("lo").hwaddr << endl;
		cout << flushSocketTools::getInterfaceInfo("eth0").address << endl;
		
		try
		{
			flushSocket st(false,PROTO_FAMILY_IPV4,TRANSFER_TYPE_STREAM);
			flushSocketExchange exch;
			std::string str;
			
			try
			{
				st.connectFrom("127.0.0.1","10.10.208.254",21,exch);

				exch.receiveStreamString(str);
				std::cout << "\ngot:\n" << str << "\n";
			}
			catch(baseEx ex)
			{
				cout << ex << "\t" << ex.line << endl;
				cout.flush();
			}
			try
			{
				st.connect("192.168.0.254",21,exch);

				exch.receiveStreamString(str);
				std::cout << "\ngot:\n" << str << "\n";
			}
			catch(baseEx ex)
			{
				cout << ex << "\t" << ex.line << endl;
				cout.flush();
			}
			try
			{
				st.connect("10.10.208.254",21,exch);

				exch.receiveStreamString(str);
				std::cout << "\ngot:\n" << str << "\n";
			}
			catch(baseEx ex)
			{
				cout << ex << "\t" << ex.line << endl;
				cout.flush();
			}
		}
		catch(baseEx ex)
		{
			cout << ex << "\t" << ex.line << endl;
			cout.flush();
		}
		
		cout << "\n-------------------------------------\n" << endl;
			
		flushSocket sock(true,PROTO_FAMILY_IPV4/*PROTO_FAMILY_IPV6*//*PROTO_FAMILY_UNIX_SOCKET*/,TRANSFER_TYPE_STREAM);
		
		__connInfo info;
		__initialAccept fake;
				
		sock.setLingerSockOption(SOCKET_HARD_CLOSE);	
		sock.blockInherited = false;
		sock.block(false);
		
		sock.bindNListen("127.0.0.1",7778,3);
		//sock.bindNListen("::",7777);
		//sock.bindNListen("./sock",10,true);
		
		flushSocketExchange conn;
		
		while(true)
		{
			if (sock.accept(fake,info))
			{
				if (sock.isBlocked())
				{
					std::cout << "PARENT BLOCKED\n";
					cout.flush();
				}
					
				conn.init(fake);
				
				process(conn);
			}
		}
		
		//flushSocketTools::setLocalName("BUBU");
		
		cout << flushSocketTools::getLocalName() << endl;
		cout << flushSocketTools::getHostInfo("192.168.0.1").addresses[0] << endl;
		
		flushSocketTools fst;
		cout << fst.getHostInfo("google.com").addresses[0] << endl;
		
	}
	catch(baseEx ex)
	{
		cout << ex << "\t" << ex.line << endl;
		cout.flush();
	}
	
	return 0;
}
