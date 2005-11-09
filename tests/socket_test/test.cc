#include <flushSocket.h>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{

	
	try
	{
		flushSocket sock(1,PROTO_FAMILY_IPV4,TRANSFER_TYPE_STREAM,6);
		sock.init();
		//sock.bindNListen("127.0.0.1",7777);
		sock.connect("127.0.0.1",21);
		
		cout << flushSocket::getHostInfo("192.168.0.1").addresses[0];
	}
	catch(baseEx ex)
	{
		cout << ex.baseErrstr << "\t" << ex.line << endl;
	}
	return 0;
}
