#include <baseEx.h>
#include <flushSocket.h>
#include <flushSocketTools.h>
#include <flushNBA.h>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{
	try
	{						
		flushSocket sock(true,PROTO_FAMILY_IPV4/*PROTO_FAMILY_IPV6*//*PROTO_FAMILY_UNIX_SOCKET*/,TRANSFER_TYPE_STREAM);
		
		__initialAccept fake;
				
		sock.bindNListen("127.0.0.1",7778,1);
		//sock.bindNListen("::",7777);
		//sock.bindNListen("./sock",10,true);

		sock.setSockOption(SOCKET_REUSE_ADDRESS,true);
		sock.setLingerSockOption(SOCKET_HARD_CLOSE);	
		sock.blockInherited = true;
		sock.block(false);
				
		flushSocketExchange conn;

		flushNBA nb;
		
		while(true)
		{
			if (sock.accept(fake))
			{
				
					
				conn.init(fake);
				
				int pos = nb.addFlush(conn);
				
				string data;
				
				while (true)
				{
					if (nb.isReadable(pos))
					{
						conn.receiveStreamString(data);
						cout << data << endl;
					}
				}
			}
		}
		
	}
	catch(baseEx ex)
	{
		cout << ex << "\t" << ex.file << "\t" << ex.line << endl;
		cout.flush();
	}
	
	return 0;
}
