
#include <libdodo/baseEx.h>
#include <libdodo/ioSocket.h>
#include <libdodo/ioSocketTools.h>
#include <libdodo/types.h>

#include <iostream>

using namespace dodo;

using namespace std;

void
process(ioSocketExchange fse)
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
	fse.writeString("dasdasd");
	
	dodoString rec = "";
	try
	{
		fse.readString(rec);
		
		cout << rec << rec.size() << endl;
		cout.flush();
		if (rec.compare("exit")==0)
		{
			exit(0);
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
}

int main(int argc, char **argv)
{
	try
	{	
		dodoStringArray ifaces = ioSocketTools::getInterfacesNames();
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
		
		try
		{
			ioSocket st(false,IOSOCKETOPTIONS_PROTO_FAMILY_IPV4,IOSOCKETOPTIONS_TRANSFER_TYPE_STREAM);
			ioSocketExchange exch;
			dodoString str;
			
			try
			{
				st.connectFrom("127.0.0.1","127.0.0.1",21,exch);

				exch.readStreamString(str);
				std::cout << "\ngot:\n" << str << "\n";
			}
			catch(baseEx ex)
			{
				cout << (string)ex << "\t" << ex.line << endl;
				cout.flush();
			}
		}
		catch(baseEx ex)
		{
			cout << (string)ex << "\t" << ex.line << endl;
			cout.flush();
		}
			
		/*ioSocket sock(true,IOSOCKETOPTIONS_PROTO_FAMILY_IPV4,IOSOCKETOPTIONS_TRANSFER_TYPE_STREAM);
		//ioSocket sock(true,IOSOCKETOPTIONS_PROTO_FAMILY_IPV6,IOSOCKETOPTIONS_TRANSFER_TYPE_STREAM);
		//ioSocket sock(true,IOSOCKETOPTIONS_PROTO_FAMILY_UNIX_SOCKET,IOSOCKETOPTIONS_TRANSFER_TYPE_STREAM);
		
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
					cout.flush();
				}
					
				conn.init(fake);
				
				process(conn);
			}
		}*/
		
		cout << ioSocketTools::getLocalName() << endl;
		
		cout << ioSocketTools::getHostInfo("google.com").name << " : " << endl;
		
		dodoStringArray addresses = ioSocketTools::getHostInfo("google.com").addresses;
		dodoStringArray::iterator i = addresses.begin(), j = addresses.end();
		for (;i!=j;++i)
			cout << *i << endl;
		
	}
	catch(baseEx ex)
	{
		cout << (string)ex << "\t" << ex.line << "\t" << ex.file << endl;
		cout.flush();
	}
	
	return 0;
}
