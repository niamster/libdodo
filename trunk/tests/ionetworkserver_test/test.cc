/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>
#include <stdlib.h>

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
	fse.writeStream("session");

	dodoString rec = "";
	try
	{
		rec = fse.read();

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
		cout << "Smth happened!" << (dodoString)ex << endl;
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

		server sock(CONNECTION_PROTO_FAMILY_IPV4,CONNECTION_TRANSFER_TYPE_STREAM);
		//server sock(CONNECTION_PROTO_FAMILY_IPV6,CONNECTION_TRANSFER_TYPE_STREAM);
		//server sock(CONNECTION_PROTO_FAMILY_UNIX_SOCKET,CONNECTION_TRANSFER_TYPE_STREAM);

		__peerInfo__ info;
		__initialAccept__ fake;

		sock.serve("127.0.0.1",7778,3);
		//sock.serve("::",7777);
		//sock.serve("./sock",10,true);

		sock.blockInherited = false;
		sock.block(false);

		sock.setLingerOption(CONNECTION_LINGEROPTION_HARD_CLOSE);

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
		cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}
