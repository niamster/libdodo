#include <flushSocket.h>

using namespace dodo;

using namespace std;

void copyTest0(flushSocketExchange fse)
{
	cout << fse.getOutBufferSize() << endl;
}

void copyTest1(flushSocketExchange &fse)
{
	cout << fse.getOutBufferSize() << endl;
}

void copyTest2(flushSocketExchange *fse)
{
	cout << fse->getOutBufferSize() << endl;
}

void process(flushSocketExchange &fse)
{
	cout << fse.getOutTimeout() << endl;
}

int main(int argc, char **argv)
{

#define DATAGRAM

#ifndef DATAGRAM
	#define CONNECTION_LIMIT 2
#else
	#define CONNECTION_LIMIT 1
#endif

	try
	{
		flushSocket sock(CONNECTION_LIMIT,PROTO_FAMILY_IPV4,TRANSFER_TYPE_DATAGRAM);
		//sock.bindNListen("127.0.0.1",7777);
		
/*		flushSocketExchange ex1;
		copyTest0(ex1);///ok
		copyTest1(ex1);///ok
		copyTest2(&ex1);///ok*/
		
		
/*		sock.connect("127.0.0.1",21,ex1);
		ex1.close();
		
		sock.connect("127.0.0.1",21,ex1);

		flushSocketExchange ex2;
		flushSocketExchange ex2 = ex1;
		sock.connect("127.0.0.1",21,ex2);
*/
		__connInfo info;
		__initialAccept fake;
		sock.bindNListen("192.168.0.201",7777);
		flushSocketExchange conn1[CONNECTION_LIMIT];
		int i = 0;
		
		while (i<CONNECTION_LIMIT)
		{
			if (sock.accept(fake,info))
			{
				cout << i << "?\n";
				conn1[i++].init(fake);
				cout << i << "!\n";
				//conn1[i++].setOutTimeout(10);
				//process(conn1);
				//cout << info.port;
			}
		}
		
		
		//flushSocket::setLocalName("BUBU");
		
		cout << flushSocket::getLocalName();
		cout << flushSocket::getHostInfo("192.168.0.1").addresses[0];
	}
	catch(baseEx ex)
	{
		cout << ex.baseErrstr << "\t" << ex.line << endl;
	}
	return 0;
}
