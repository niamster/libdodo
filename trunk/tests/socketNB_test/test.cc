#include <libdodo/baseEx.h>
#include <libdodo/ioSocket.h>
#include <libdodo/ioSocketTools.h>
#include <libdodo/ioNBA.h>

#include <iostream>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{
	try
	{						
		ioSocket sock(true,IOSOCKETOPTIONS_PROTO_FAMILY_IPV4/*IOSOCKETOPTIONS_PROTO_FAMILY_IPV6*//*IOSOCKETOPTIONS_PROTO_FAMILY_UNIX_SOCKET*/,IOSOCKETOPTIONS_TRANSFER_TYPE_STREAM);
		
		__initialAccept fake;

		sock.setSockOption(IOSOCKETOPTIONS_SOCKET_REUSE_ADDRESS,true);
		sock.setLingerSockOption(IOSOCKETOPTIONS_SOCKET_HARD_CLOSE);	
		sock.blockInherited = true;
		sock.block(false);
						
		sock.bindNListen("127.0.0.1",7778,1);
		//sock.bindNListen("::",7777);
		//sock.bindNListen("./sock",10,true);
				
		ioSocketExchange conn;

		ioNBA nb;
		
		while(true)
		{
			if (sock.accept(fake))
			{
				conn.init(fake);
				
				if (conn.isBlocked())
					cout << "is Blocked" << endl;
				else
					cout << "is not Blocked" << endl;
				
				int pos = nb.addFlush(conn);
				
				dodoString data;
				
				while (true)
				{
					if (nb.isReadable(pos))
					{
						conn.readStreamString(data);
						cout << data << endl;
					}
				}
			}
		}
		
	}
	catch(baseEx ex)
	{
		cout << (string)ex << "\t" << ex.file << "\t" << ex.line << endl;
		cout.io();
	}
	
	return 0;
}
