/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/baseEx.h>
#include <libdodo/ioNetworkSslClient.h>
#include <libdodo/types.h>

#include <iostream>

using namespace dodo;
using namespace io::network::ssl;

using namespace std;

int main(int argc, char **argv)
{
	try
	{
		dodoString host = "sourceforge.net";

		cout << tools::network::getHostPrimaryIp(host) << endl;

		client st(io::network::OPTIONS_PROTO_FAMILY_IPV4, io::network::OPTIONS_TRANSFER_TYPE_STREAM);
		exchange exch;
		dodoString str;

		st.connect(tools::network::getHostPrimaryIp(host), 443, exch);

		exch.writeStreamString("GET / HTTP/1.0\r\n");
		exch.writeStreamString("Host: " + host + "\r\n");
		exch.writeStreamString("\r\n");

		while (true)
		{
			exch.readStreamString(str);
			if (str.size() == 0)
				break;
			cout << str;
		}
	}
	catch (baseEx ex)
	{
		cout << (string)ex << "\t" << ex.line << "\t" << ex.file << endl;
		cout.flush();
	}

	return 0;
}
