/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;
using namespace io::network;

using namespace std;

int main(int argc, char **argv)
{
	try
	{
		server sock(CONNECTION_PROTO_FAMILY_IPV4, CONNECTION_TRANSFER_TYPE_STREAM);

		__initialAccept__ accepted;

		sock.serve("127.0.0.1", 7778, 1);
		sock.setOption(CONNECTION_OPTION_REUSE_ADDRESS, true);
		sock.setLingerOption(CONNECTION_LINGEROPTION_HARD_CLOSE);
		sock.blockInherited = true;
		sock.block(false);

		io::event nb;

		char trimSym[] = { '\r', '\n' };

		while (true)
		{
			if (sock.accept(accepted))
			{
				exchange ex(accepted);

				if (ex.isBlocked())
					cout << "is Blocked" << endl;
				else
					cout << "is not Blocked" << endl;

				int pos = nb.addChannel(ex);

				dodoString data;

				while (true)
				{
					if (nb.isReadable(pos))
					{
						data = ex.readStream();
						cout << "'" << tools::string::trim(data, trimSym, 2) << "'" << endl;

						if (tools::string::trim(data, trimSym, 2) == "exit")
							tools::os::die(data);
					}
				}
			}
		}

	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << "\t" << ex.file << "\t" << ex.line << endl;
	}

	return 0;
}
