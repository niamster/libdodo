#include <flushSocket.h>
#include <flushSTD.h>
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

void 
hook(dodoBase *base, 
	void *yep)
{
	flushSTD *st = dynamic_cast<flushSTD *>(base->getSelf());
	if (st->operType == FLUSHSTD_OPER_WRITE)
	{
		st->buffer = "1234567890";
		st->outSize = 10;
	}
}

void process(flushSocketExchange &fse)
{
	
	flushSTD st;
	st.outSize = 3;
	st.addPreExec(&hook,NULL);
	fse.inSize = 2;
	fse.setInBufferSize(1);
	st.write("qs1");
//	cout << fse.getOutTimeout() << endl;
	
	fse.outSize = 7;
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
		flushSocket sock(CONNECTION_LIMIT,PROTO_FAMILY_IPV4,TRANSFER_TYPE_STREAM);
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
		sock.setSockOption(SOCKET_REUSE_ADDRESS,true);
		sock.setLingerSockOption(SOCKET_HARD_CLOSE);
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
