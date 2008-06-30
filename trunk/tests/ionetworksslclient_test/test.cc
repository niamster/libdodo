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

#ifdef OPENSSL_EXT

using namespace io::network::ssl;

#endif

using namespace std;

int main(int argc, char **argv)
{
	try
	{
#ifdef OPENSSL_EXT

		dodoString host = "sourceforge.net";

		cout << tools::network::getHostPrimaryIp(host) << endl;

		client st(io::network::OPTIONS_PROTO_FAMILY_IPV4, io::network::OPTIONS_TRANSFER_TYPE_STREAM);
		
		io::ssl::__certificates certs;
		certs.ca = "host.pem";
		
		//certs.cert = "host.cert";
		//certs.key = "host.key";
		//certs.keyType = KEYTYPE_PKEY;
	
		//certs.caPath = "./";

		st.setSertificates(certs);

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

#endif
	}
	catch (baseEx ex)
	{
		cout << (string)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}
