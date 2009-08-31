/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
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

		cout << host << ":" << tools::network::hostPrimaryIp(host) << endl;

		client st(io::network::connection::PROTOCOL_FAMILY_IPV4, io::network::connection::TRANSFER_STREAM);

		io::ssl::__certificates__ certs;
		certs.ca = "host.pem";

		//certs.cert = "host.cert";
		//certs.key = "host.key";
		//certs.keyType = KEYTYPE_PKEY;

		//certs.caPath = "./";

		st.setSertificates(certs);

		exchange ex;
		dodoString str;

		st.connect(tools::network::hostPrimaryIp(host), 443, ex);

		str = "GET / HTTP/1.1\r\n";
		ex.outSize = str.size();
		ex.write(str);

		str = "Host: " + host + "\r\n";
		ex.outSize = str.size();
		ex.write(str);

		str = "Connection: Close\r\n";
		ex.outSize = str.size();
		ex.write(str);

		str = "User-Agent: " PACKAGE_NAME "/" PACKAGE_VERSION "\r\n\r\n";
		ex.outSize = str.size();
		ex.write(str);

		str = ex.readString();

		cout << str;
#endif
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}
