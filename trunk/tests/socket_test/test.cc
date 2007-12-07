#include <libdodo/baseEx.h>
#include <libdodo/ioSocket.h>
#include <libdodo/ioSocketTools.h>

#include <iostream>

using namespace dodo;

using namespace std;

void
process(ioSocketExchange fse)
{
	
	if (fse.isBlocked())
	{
		std::cout << "CHILD BLOCKED\n";
		cout.io();
	}
	else
	{
		std::cout << "CHILD UNBLOCKED\n";
		cout.io();
	}
	
	fse.inSize = 4;
	fse.setInBufferSize(1);
	fse.setOutBufferSize(1);
	
	fse.outSize = 7;
	//fse.writeStreamString("dasdasd");
	fse.writeString("dasdasd");
	
	dodoString rec = "";
	try
	{
		//fse.readStreamString(rec);
		fse.readString(rec);
		
		cout << rec << rec.size() << endl;
		cout.io();
		if (rec.compare("exit")==0)
		{
			exit(0);
		}
	}
	catch (baseEx ex)
	{
		cout << "Smth happened!" << (string)ex << endl;
		cout.io();
	}
	catch(...)
	{
		cout << "Smth happened!" << endl;
		cout.io();		
	}
}

int main(int argc, char **argv)
{
	try
	{	
		dodoStringArr ifaces = ioSocketTools::getInterfacesNames();
		for (unsigned int i(0);i<ifaces.size();i++)
		{
			try
			{
				cout << ifaces[i] << ":\t" << ioSocketTools::getInterfaceInfo(ifaces[i]).hwaddr << endl;	
				cout << ifaces[i] << ":\t" << ioSocketTools::getInterfaceInfo(ifaces[i]).broadcast << endl;	
				cout << ifaces[i] << ":\t" << ioSocketTools::getInterfaceInfo(ifaces[i]).netmask << endl;	
			}
			catch (baseEx ex)
			{
				cout << ifaces[i] << ":\t" << (string)ex << endl;
			}
		}
		
		cout << ioSocketTools::getInterfaceInfo("lo").hwaddr << endl;
		cout << ioSocketTools::getInterfaceInfo("eth1").address << endl;
		
		try
		{
			ioSocket st(false,IOSOCKETOPTIONS_PROTO_FAMILY_IPV4,IOSOCKETOPTIONS_TRANSFER_TYPE_STREAM);
			ioSocketExchange exch;
			dodoString str;
			
			try
			{
				st.connectFrom("127.0.0.1","10.10.208.254",21,exch);

				exch.readStreamString(str);
				std::cout << "\ngot:\n" << str << "\n";
			}
			catch(baseEx ex)
			{
				cout << (string)ex << "\t" << ex.line << endl;
				cout.io();
			}
			try
			{
				st.connect("192.168.0.254",21,exch);

				exch.readStreamString(str);
				std::cout << "\ngot:\n" << str << "\n";
			}
			catch(baseEx ex)
			{
				cout << (string)ex << "\t" << ex.line << endl;
				cout.io();
			}
			try
			{
				st.connect("10.10.208.254",21,exch);

				exch.readStreamString(str);
				std::cout << "\ngot:\n" << str << "\n";
			}
			catch(baseEx ex)
			{
				cout << (string)ex << "\t" << ex.line << endl;
				cout.io();
			}
		}
		catch(baseEx ex)
		{
			cout << (string)ex << "\t" << ex.line << endl;
			cout.io();
		}
		
		cout << "\n-------------------------------------\n" << endl;
			
		ioSocket sock(true,IOSOCKETOPTIONS_PROTO_FAMILY_IPV4/*IOSOCKETOPTIONS_PROTO_FAMILY_IPV6*//*IOSOCKETOPTIONS_PROTO_FAMILY_UNIX_SOCKET*/,IOSOCKETOPTIONS_TRANSFER_TYPE_STREAM);
		
		__connInfo info;
		__initialAccept fake;
				
		sock.setLingerSockOption(IOSOCKETOPTIONS_SOCKET_HARD_CLOSE);	
		sock.blockInherited = false;
		sock.block(false);
		
		sock.bindNListen("127.0.0.1",7778,3);
		//sock.bindNListen("::",7777);
		//sock.bindNListen("./sock",10,true);
		
		ioSocketExchange conn;
		
		while(true)
		{
			if (sock.accept(fake,info))
			{
				if (sock.isBlocked())
				{
					std::cout << "PARENT BLOCKED\n";
					cout.io();
				}
					
				conn.init(fake);
				
				process(conn);
			}
		}
		
		//ioSocketTools::setLocalName("BUBU");
		
		cout << ioSocketTools::getLocalName() << endl;
		cout << ioSocketTools::getHostInfo("192.168.0.1").addresses[0] << endl;
		
		ioSocketTools fst;
		cout << fst.getHostInfo("google.com").addresses[0] << endl;
		
	}
	catch(baseEx ex)
	{
		cout << (string)ex << "\t" << ex.line << "\t" << ex.file << endl;
		cout.io();
	}
	
	return 0;
}
