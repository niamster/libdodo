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

int main(int argc, char **argv)
{

	
	try
	{
		flushSocket sock(/*1,*/PROTO_FAMILY_IPV4,TRANSFER_TYPE_STREAM,6);
		//sock.bindNListen("127.0.0.1",7777);
		
		flushSocketExchange ex1;
		///copyTest0(ex1); error
		copyTest1(ex1);///ok
		copyTest2(&ex1);///ok
		
		
		sock.connect("127.0.0.1",21,ex1);
		ex1.close();
		
		sock.connect("127.0.0.1",21,ex1);

		flushSocketExchange ex2;
		///flushSocketExchange ex2 = ex1; err
		sock.connect("127.0.0.1",21,ex2);
		
		
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
