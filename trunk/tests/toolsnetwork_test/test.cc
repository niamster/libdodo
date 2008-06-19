/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/baseEx.h>
#include <libdodo/ioNetwork.h>
#include <libdodo/toolsNetwork.h>
#include <libdodo/types.h>

#include <iostream>

using namespace dodo;
using namespace io::network;

using namespace std;

int main(int argc, char **argv)
{
	try
	{
		cout << tools::network::getHostPrimaryIp("google.com") << endl;

		dodoStringArray ifaces = tools::network::getInterfacesNames();
		for (unsigned int i(0); i < ifaces.size(); i++)
		{
			try
			{
				cout << ifaces[i] << ":\t" << tools::network::getInterfaceInfo(ifaces[i]).hwaddr << endl;
				cout << ifaces[i] << ":\t" << tools::network::getInterfaceInfo(ifaces[i]).broadcast << endl;
				cout << ifaces[i] << ":\t" << tools::network::getInterfaceInfo(ifaces[i]).netmask << endl;
			}
			catch (baseEx ex)
			{
				cout << ifaces[i] << ":\t" << (string)ex << endl;
			}
		}

		cout << tools::network::getInterfaceInfo("lo").hwaddr << endl;

		cout << tools::network::getLocalName() << endl;

		cout << tools::network::getHostInfo("google.com").name << " : " << endl;

		dodoStringArray addresses = tools::network::getHostInfo("google.com").addresses;
		dodoStringArray::iterator i = addresses.begin(), j = addresses.end();
		for (; i != j; ++i)
			cout << *i << endl;

	}
	catch (baseEx ex)
	{
		cout << (string)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}
