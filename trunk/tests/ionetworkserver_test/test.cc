/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/exceptionBasic.h>
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
		if (rec.compare("exit") == 0)
		{
			fse.close();

			exit(0);
		}
	}
	catch (dodo::exception::basic ex)
	{
		cout << "Smth happened!" << (string)ex << endl;
		cout.flush();
	}
	catch (...)
	{
		cout << "Smth happened!" << endl;
		cout.flush();
	}
}

int main(int argc, char **argv)
{
	try
	{

		server sock(OPTIONS_PROTO_FAMILY_IPV4,OPTIONS_TRANSFER_TYPE_STREAM);
		//server sock(OPTIONS_PROTO_FAMILY_IPV6,OPTIONS_TRANSFER_TYPE_STREAM);
		//server sock(OPTIONS_PROTO_FAMILY_UNIX_SOCKET,OPTIONS_TRANSFER_TYPE_STREAM);

		__peerInfo info;
		__initialAccept fake;

		sock.bindNListen("127.0.0.1",7778,3);
		//sock.bindNListen("::",7777);
		//sock.bindNListen("./sock",10,true);
		
		sock.blockInherited = false;
		sock.block(false);
		
		sock.setLingerOption(OPTIONS_LINGEROPTION_HARD_CLOSE);

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
		}
	}
	catch (dodo::exception::basic ex)
	{
		cout << (string)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}
