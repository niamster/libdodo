/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>
#include <stdlib.h>

using namespace dodo;
using namespace std;

#ifdef OPENSSL_EXT
using namespace io::network::ssl;

#ifndef IO_WO_XEXEC
void
hook(xexec::__collected_data__ *odata,
	 short int type,
	 void *udata)
{
	io::channel::__collected_data__ *st = (io::channel::__collected_data__ *)odata;

	cout << "In hook: " << st->buffer << endl;
	/* cout << dynamic_cast<exchange *>(st->executor)->isAlive() << endl; */ /* FIXME: issue #61 */
}
#endif

void
process(exchange &ex)
{
#ifndef IO_WO_XEXEC
	ex.addXExec(xexec::ACTION_PREEXEC, ::hook, NULL);
	ex.addXExec(xexec::ACTION_POSTEXEC, ::hook, NULL);
#endif

	ex.inSize = 4;
	ex.setInBufferSize(1);
	ex.setOutBufferSize(1);

	ex.outSize = 7;
	ex.writeString("test\n");

	if (ex.isAlive())
		ex.writeString("Alive");

	dodoString str = "";
	try
	{
		str = ex.read();

		cout << str << ":" << str.size() << endl;
		cout.flush();
		if (str.compare("exit") == 0)
		{
			ex.close();

			exit(0);
		}
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
		cout.flush();
	}
}
#endif

int main(int argc, char **argv)
{
	try
	{
#ifdef OPENSSL_EXT
		server sock(io::network::connection::PROTOCOL_FAMILY_IPV4, io::network::connection::TRANSFER_STREAM);

		io::network::connection::__peer__ info;
		io::network::ssl::exchange::__init__ accepted;

		sock.serve("127.0.0.1", 7778, 3);

		io::ssl::__certificates__ certs;
		certs.ca = "host.pem";
		certs.cipher = "RC4-MD5";

		sock.setSertificates(certs);

		while(true)
		{
			if (sock.accept(accepted, info))
			{
				if (sock.isBlocked())
				{
					cout << "Blocked\n";
					cout.flush();
				}

				exchange ex(accepted);

				process(ex);
			}
		}
#endif
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}
