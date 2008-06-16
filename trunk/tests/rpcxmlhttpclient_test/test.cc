
#include <libdodo/baseEx.h>
#include <libdodo/rpcXmlHttpClient.h>
#include <libdodo/types.h>

#include <iostream>

using namespace dodo;
using namespace rpc;

using namespace std;


int main(int argc, char **argv)
{
	try
	{
		rpc::xml::httpClient client;

		client.setUrl("http://localhost/libdodo/rpcxmlcgiserver_test/test.cgi");

		method method;
		value argument;

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

		response resp = client.receiveResponse();

		cout << resp.getValue().getString() << endl;
	}
	catch (baseEx ex)
	{
		cout << (string)ex << "\t" << ex.line << "\t" << ex.file << endl;
		cout.flush();
	}

	return 0;
}
