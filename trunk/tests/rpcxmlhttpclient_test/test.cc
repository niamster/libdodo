
#include <libdodo/baseEx.h>
#include <libdodo/rpcXmlHTTPClient.h>
#include <libdodo/types.h>

#include <iostream>

using namespace dodo;

using namespace std;


int main(int argc, char **argv)
{
	try
	{
		rpcXmlHTTPClient client;

		client.setUrl("http://localhost/libdodo/cgi_test/test.cgi");

		rpcMethod method;
		rpcValue argument;

		method.setName("callTest");

		argument.setString("argument");
		method.addArgument(argument);

		argument.setBoolean(true);
		method.addArgument(argument);

		argument.addStructMember("string", dodoString("string"));
		argument.addStructMember("integer", (long)10);
		method.addArgument(argument);

		argument.addArrayElement(dodoString("string"));
		argument.addArrayElement((double)10.0);
		method.addArgument(argument);

		client.sendRequest(method);

		//rpcResponse response = client.receiveResponse();
	}
	catch(baseEx ex)
	{
		cout << (string)ex << "\t" << ex.line << "\t" << ex.file << endl;
		cout.flush();
	}
	
	return 0;
}
