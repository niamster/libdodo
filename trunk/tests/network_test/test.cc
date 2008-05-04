
#include <libdodo/baseEx.h>
#include <libdodo/ioNetwork.h>
#include <libdodo/toolsNetwork.h>
#include <libdodo/types.h>

#include <iostream>

using namespace dodo;
using namespace io::network;

using namespace std;

void
process(exchange fse)
{
	
	if (fse.isBlocked())
	{
		cout << "CHILD BLOCKED\n";
		cout.flush();
	}
	else
	{
		cout << "CHILD UNBLOCKED\n";
		cout.flush();
	}
	
	fse.inSize = 4;
	fse.setInBufferSize(1);
	fse.setOutBufferSize(1);
	
	fse.outSize = 7;
	fse.writeString("session");
	
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
		cout << toolsNetwork::getHostPrimaryIp("google.com") << endl;

		dodoStringArray ifaces = toolsNetwork::getInterfacesNames();
		for (unsigned int i(0);i<ifaces.size();i++)
		{
			try
			{
				cout << ifaces[i] << ":\t" << toolsNetwork::getInterfaceInfo(ifaces[i]).hwaddr << endl;	
				cout << ifaces[i] << ":\t" << toolsNetwork::getInterfaceInfo(ifaces[i]).broadcast << endl;	
				cout << ifaces[i] << ":\t" << toolsNetwork::getInterfaceInfo(ifaces[i]).netmask << endl;	
			}
			catch (baseEx ex)
			{
				cout << ifaces[i] << ":\t" << (string)ex << endl;
			}
		}
		
		cout << toolsNetwork::getInterfaceInfo("lo").hwaddr << endl;
		
		try
		{
			client st(OPTIONS_PROTO_FAMILY_IPV4,OPTIONS_TRANSFER_TYPE_STREAM);
			exchange exch;
			dodoString str;
			
			try
			{
				st.connectFrom("127.0.0.1","127.0.0.1",21,exch);

				exch.readStreamString(str);
				cout << "\ngot:\n" << str << "\n";
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
			
		/*server sock(OPTIONS_PROTO_FAMILY_IPV4,OPTIONS_TRANSFER_TYPE_STREAM);
		//server sock(OPTIONS_PROTO_FAMILY_IPV6,OPTIONS_TRANSFER_TYPE_STREAM);
		//server sock(OPTIONS_PROTO_FAMILY_UNIX_SOCKET,OPTIONS_TRANSFER_TYPE_STREAM);
		
		__connInfo info;
		__initialAccept fake;
				
		sock.setLingerSockOption(IONETWORKOPTIONS_SOCKET_HARD_CLOSE);	
		sock.blockInherited = false;
		sock.block(false);
		
		sock.bindNListen("127.0.0.1",7778,3);
		//sock.bindNListen("::",7777);
		//sock.bindNListen("./sock",10,true);
		
		exchange conn;
		
		while(true)
		{
			if (sock.accept(fake,info))
			{
				if (sock.isBlocked())
				{
					cout << "PARENT BLOCKED\n";
					cout.flush();
				}
					
				conn.init(fake);
				
				process(conn);
			}
		}*/
		
		cout << toolsNetwork::getLocalName() << endl;
		
		cout << toolsNetwork::getHostInfo("google.com").name << " : " << endl;
		
		dodoStringArray addresses = toolsNetwork::getHostInfo("google.com").addresses;
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
