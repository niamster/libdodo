/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/baseEx.h>
#include <libdodo/ioNetworkSslServer.h>
#include <libdodo/toolsNetwork.h>
#include <libdodo/types.h>

#include <iostream>

using namespace dodo;
using namespace io::network::ssl;

using namespace std;

#ifndef IO_WO_XEXEC

void
hook(void *odata,
	 short int type,
	 void *udata)
{
	using io::network::__xexexIoNetworkExchangeCollectedData;

	__xexexIoNetworkExchangeCollectedData *st = (__xexexIoNetworkExchangeCollectedData *)odata;

	cout << st->buffer << endl;
}

#endif

void
process(exchange fse)
{
#ifndef IO_WO_XEXEC

		fse.addPreExec(&hook, NULL);
		fse.addPostExec(&hook, NULL);

#endif

	fse.inSize = 4;
	fse.setInBufferSize(1);
	fse.setOutBufferSize(1);

	fse.outSize = 7;
	fse.writeString("session");

	if (fse.isAlive())
		fse.writeStreamString("Alive!");

	dodoString rec = "";
	try
	{
		fse.readString(rec);

		cout << rec << rec.size() << endl;
		cout.flush();
		if (rec.compare("exit") == 0)
		{
			fse.close();
			
			exit(0);
		}
	}
	catch (baseEx ex)
	{
		cout << "Smth happened!" << (string)ex << endl;
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

		server sock(io::network::OPTIONS_PROTO_FAMILY_IPV4, io::network::OPTIONS_TRANSFER_TYPE_STREAM);

		io::network::__connInfo info;
		__initialAccept fake;

		sock.bindNListen("127.0.0.1",7778,3);
		
		__certificates certs;
		certs.ca = "host.pem";
		certs.cipher = "RC4-MD5";

		sock.setSertificates(certs);

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
	catch (baseEx ex)
	{
		cout << (string)ex << "\t" << ex.line << "\t" << ex.file << endl;
		cout.flush();
	}

	return 0;
}
