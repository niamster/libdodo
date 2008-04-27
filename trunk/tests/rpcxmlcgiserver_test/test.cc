#include <libdodo/baseEx.h>
#include <libdodo/rpcXmlCgiServer.h>

#include <iostream>

using namespace dodo;
using namespace cgi;

using namespace std;

rpcResponse 
handler(const dodoString &method, const dodoArray<rpcValue> &values)
{
	rpcResponse response;

	response.addArgument(dodoString("Got method: ") + method);

	return response;
}

int main(int argc, char **argv)
{		
	client provider(false);
	rpcXmlCgiServer server(provider);
	
	try
	{
		server.setHandler("callTest", handler);

		server.serve();
	}
	catch(baseEx ex)
	{
		cout << ex.message << "\t" << ex.baseErrstr << endl;
	}	
		
	return 0;
}
