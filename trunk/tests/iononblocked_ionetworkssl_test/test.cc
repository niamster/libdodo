/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/exceptionBasic.h>
#include <libdodo/toolsOs.h>
#include <libdodo/ioNetworkSslServer.h>
#include <libdodo/ioEvent.h>

#include <iostream>

using namespace dodo;

#ifdef OPENSSL_EXT

using namespace io::network::ssl;

#endif

using namespace std;

int main(int argc, char **argv)
{
	try
	{
#ifdef OPENSSL_EXT

		server sock(io::network::CONNECTION_PROTO_FAMILY_IPV4, io::network::CONNECTION_TRANSFER_TYPE_STREAM);

		__initialAccept fake;

		sock.serve("127.0.0.1", 7778, 1);
		sock.setOption(io::network::CONNECTION_OPTION_REUSE_ADDRESS, true);
		sock.setLingerOption(io::network::CONNECTION_LINGEROPTION_HARD_CLOSE);
		sock.blockInherited = true;
		sock.block(false);
		
		io::ssl::__certificates certs;
		certs.ca = "host.pem";
		certs.cipher = "RC4-MD5";

		sock.setSertificates(certs);

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
						data = conn.readStream();
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
#endif
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << "\t" << ex.file << "\t" << ex.line << endl;
	}

	return 0;
}
