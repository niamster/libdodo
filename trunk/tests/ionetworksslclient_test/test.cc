/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/exceptionBasic.h>
#include <libdodo/ioNetworkSslClient.h>
#include <libdodo/toolsNetwork.h>
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

		dodoString host = "kernel.org";

		cout << tools::network::getHostPrimaryIp(host) << endl;

		client st(io::network::CONNECTION_PROTO_FAMILY_IPV4, io::network::CONNECTION_TRANSFER_TYPE_STREAM);
		
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

		str = "GET / HTTP/1.1\r\n";
		exch.outSize = str.size();
		exch.write(str);

		str = "Host: " + host + "\r\n";
		exch.outSize = str.size();
		exch.write(str);

		str = "Connection: Close\r\n";
		exch.outSize = str.size();
		exch.write(str);

		str = "User-Agent: " PACKAGE_NAME "/" PACKAGE_VERSION "\r\n\r\n";
		exch.outSize = str.size();
		exch.write(str);

		str = exch.readStream();

		cout << str;

#endif
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}
