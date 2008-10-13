/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/exceptionBasic.h>
#include <libdodo/cgiBasicExchange.h>
#include <libdodo/rpcJsonCgiServer.h>

#include <iostream>

using namespace dodo;
using namespace rpc;

using namespace std;

response
handler(const dodoString &method, const dodoArray<value> &values, const void *idata, void *odata)
{
	response resp;

	resp.addArgument(dodoString("Got method: ") + method + "\n");
	resp.addArgument(dodoString("Amount of values: ") + tools::string::ulToString(values.size()) + "\n");

	return resp;
}

int main(int argc, char **argv)
{
	dodo::cgi::basic::exchange cgiio;
	dodo::cgi::dialogue provider(cgiio, true);

	rpc::json::cgi::server srv(provider);

	try
	{
		srv.setHandler("callTest", ::handler);

		srv.serve();
	}
	catch (dodo::exception::basic ex)
	{
		cout << ex.message << "\t" << ex.baseErrstr << endl;
	}

	return 0;
}
