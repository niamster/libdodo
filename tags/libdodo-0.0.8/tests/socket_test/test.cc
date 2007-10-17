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
	fse.inSize = 2;
	fse.setInBufferSize(1);
	
//	fse.outSize = 7;
//	fse.autoOutSize = false;
	fse.sendString("dasdasd");
	
	std::string q;
	try
	{
		fse.recieveString(q);
		cout << q << endl;
	}
	catch (baseEx ex)
	{
		cout << ex << endl;
	}
	
}

int main(int argc, char **argv)
{

//#define DATAGRAM

#ifndef DATAGRAM
	#define CONNECTION_LIMIT 2
#else
	#define CONNECTION_LIMIT 1
#endif


	try
	{		
		flushSocket sock(true,/*PROTO_FAMILY_IPV4*/PROTO_FAMILY_IPV6/*PROTO_FAMILY_UNIX_SOCKET*/,TRANSFER_TYPE_STREAM);
		
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
		sock.setSockOption(SOCKET_REUSE_ADDRESS,true);
		sock.setLingerSockOption(SOCKET_HARD_CLOSE);
		
		sock.bindNListen("127.0.0.1",7777,CONNECTION_LIMIT);
		//sock.bindNListen("::",7777);
		//sock.bindNListen("./sock",true);
		
		flushSocketExchange conn1[CONNECTION_LIMIT];
		int i = 0;
		
		#ifdef DATAGRAM
			while(true)
		#else
			while (i<CONNECTION_LIMIT+1)
		#endif
		{
			if (sock.accept(fake,info))
			{
				conn1[i].init(fake);
				#ifdef DATAGRAM
					process(conn1[i]);
				#else
					process(conn1[i++]);
				#endif
				cout << info.port << endl;
			}
		}
		
		
		//flushSocket::setLocalName("BUBU");
		
		cout << flushSocket::getLocalName() << endl;
		cout << flushSocket::getHostInfo("192.168.0.1").addresses[0] << endl;
		
		flushSocket *pointer[10];
		pointer[0] = &sock;
		cout << pointer[0]->getHostInfo("elessar.mu").addresses[0] << endl;
		
	}
	catch(baseEx ex)
	{
		cout << ex << "\t" << ex.line << endl;
	}
	return 0;
}
