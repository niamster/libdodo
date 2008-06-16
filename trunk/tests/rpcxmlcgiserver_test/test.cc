/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/baseEx.h>
#include <libdodo/rpcXmlCgiServer.h>

#include <iostream>

using namespace dodo;
using namespace rpc;

using namespace std;

response
handler(const dodoString &method, const dodoArray<value> &values)
{
	response resp;

	resp.addArgument(dodoString("Got method: ") + method);

	return resp;
}

int main(int argc, char **argv)
{
	cgi::server provider(false);
	rpc::xml::cgiServer srv(provider);

	try
	{
		srv.setHandler("callTest", ::handler);

		srv.serve();
	}
	catch (baseEx ex)
	{
		cout << ex.message << "\t" << ex.baseErrstr << endl;
	}

	return 0;
}
