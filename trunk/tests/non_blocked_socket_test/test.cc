#include <libdodo/baseEx.h>
#include <libdodo/ioNetwork.h>
#include <libdodo/ioNetworkTools.h>
#include <libdodo/ioNonBlockedAccess.h>

#include <iostream>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{
	try
	{						
		ioNetwork sock(true,IONETWORKOPTIONS_PROTO_FAMILY_IPV4,IONETWORKOPTIONS_TRANSFER_TYPE_STREAM);
		
		__initialAccept fake;

		sock.setOption(IONETWORKOPTIONS_OPTION_REUSE_ADDRESS,true);
		sock.setLingerOption(IONETWORKOPTIONS_LINGEROPTION_HARD_CLOSE);	
		sock.blockInherited = true;
		sock.block(false);
						
		sock.bindNListen("127.0.0.1",7778,1);
				
		ioNetworkExchange conn;

		ioNonBlockedAccess nb;
		
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
		cout.flush();
	}
	
	return 0;
}
