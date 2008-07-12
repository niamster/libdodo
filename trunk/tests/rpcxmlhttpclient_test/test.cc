/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


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

		cout << "Amount of values: " << resp.getValues().size() << endl;
		cout << "First value: " << resp.getValue().getString() << endl;
		cout << "Second value: " << resp.getValue(1).getString() << endl;
	}
	catch (baseEx ex)
	{
		cout << (string)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}
