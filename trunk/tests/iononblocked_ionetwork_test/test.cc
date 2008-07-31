/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/exceptionBasic.h>
#include <libdodo/toolsOs.h>
#include <libdodo/ioNetwork.h>
#include <libdodo/toolsNetwork.h>
#include <libdodo/ioEvent.h>

#include <iostream>

using namespace dodo;
using namespace io::network;

using namespace std;

int main(int argc, char **argv)
{
	try
	{
		server sock(OPTIONS_PROTO_FAMILY_IPV4, OPTIONS_TRANSFER_TYPE_STREAM);

		__initialAccept fake;

		sock.bindNListen("127.0.0.1", 7778, 1);
		sock.setOption(OPTIONS_OPTION_REUSE_ADDRESS, true);
		sock.setLingerOption(OPTIONS_LINGEROPTION_HARD_CLOSE);
		sock.blockInherited = true;
		sock.block(false);

		exchange conn;

		io::event nb;

		char trimSym[] = { '\r', '\n' };

		while (true)
		{
			if (sock.accept(fake))
			{
				conn.init(fake);

				if (conn.isBlocked())
					cout << "is Blocked" << endl;
				else
					cout << "is not Blocked" << endl;

				int pos = nb.addChannel(conn);

				dodoString data;

				while (true)
				{
					if (nb.isReadable(pos))
					{
						conn.readStreamString(data);
						cout << data << endl;

						if (tools::string::trim(data, trimSym, 2) == "exit")
							tools::os::die(data);
					}
				}
			}
		}

	}
	catch (dodo::exception::basic ex)
	{
		cout << (string)ex << "\t" << ex.file << "\t" << ex.line << endl;
	}

	return 0;
}
