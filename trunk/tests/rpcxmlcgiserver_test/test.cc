#include <libdodo/baseEx.h>
#include <libdodo/rpcXmlCgiServer.h>

#include <iostream>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{		
	rpcXmlCgiServer server;
	
	try
	{
		server.serve();
	}
	catch(baseEx ex)
	{
		cout << ex.message << "\t" << ex.baseErrstr << endl;
	}	
		
	return 0;
}
