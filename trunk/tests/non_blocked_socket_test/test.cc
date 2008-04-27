#include <libdodo/baseEx.h>
#include <libdodo/toolsSystem.h>
#include <libdodo/ioNetwork.h>
#include <libdodo/toolsNetwork.h>
#include <libdodo/ioNonBlockedAccess.h>

#include <iostream>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{
	try
	{						
		ioNetworkServer sock(IONETWORKOPTIONS_PROTO_FAMILY_IPV4,IONETWORKOPTIONS_TRANSFER_TYPE_STREAM);
		
		__initialAccept fake;

		sock.bindNListen("127.0.0.1",7778,1);
		sock.setOption(IONETWORKOPTIONS_OPTION_REUSE_ADDRESS,true);
		sock.setLingerOption(IONETWORKOPTIONS_LINGEROPTION_HARD_CLOSE);	
		sock.blockInherited = true;
		sock.block(false);
				
		ioNetworkExchange conn;

		ioNonBlockedAccess nb;
		
		char trimSym[] = {'\r', '\n'};

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

						if (stringTools::trim(data, trimSym, 2) == "exit")
							toolsSystem::die(data);
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
