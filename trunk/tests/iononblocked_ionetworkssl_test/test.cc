/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/baseEx.h>
#include <libdodo/toolsOs.h>
#include <libdodo/ioNetworkSslServer.h>
#include <libdodo/ioNonBlockedAccess.h>

#include <iostream>

using namespace dodo;
using namespace io::network::ssl;

using namespace std;

int main(int argc, char **argv)
{
	try
	{
		server sock(io::network::OPTIONS_PROTO_FAMILY_IPV4, io::network::OPTIONS_TRANSFER_TYPE_STREAM);

		__initialAccept fake;

		sock.bindNListen("127.0.0.1", 7778, 1);
		sock.setOption(io::network::OPTIONS_OPTION_REUSE_ADDRESS, true);
		sock.setLingerOption(io::network::OPTIONS_LINGEROPTION_HARD_CLOSE);
		sock.blockInherited = true;
		sock.block(false);
		
		io::ssl::__certificates certs;
		certs.ca = "host.pem";
		certs.cipher = "RC4-MD5";

		sock.setSertificates(certs);

		exchange conn;

		io::nonBlockedAccess nb;

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

				int pos = nb.addFlush(conn);

				dodoString data;

				while (true)
				{
					if (nb.isReadable(pos))
					{
						conn.readStreamString(data);
						cout << data << endl;

						if (tools::string::trim(data, trimSym, 2) == "exit")
						{
							conn.close();

							tools::os::die(data);
						}
					}
				}
			}
		}

	}
	catch (baseEx ex)
	{
		cout << (string)ex << "\t" << ex.file << "\t" << ex.line << endl;
	}

	return 0;
}
