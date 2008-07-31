/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/exceptionBasic.h>
#include <libdodo/ioNetwork.h>
#include <libdodo/toolsNetwork.h>
#include <libdodo/types.h>

#include <iostream>

using namespace dodo;
using namespace tools;

using namespace std;

int main(int argc, char **argv)
{
	try
	{
		__hostInfo addr;

		addr = network::getHostInfo("localhost");
		cout << "Name: " << addr.name << endl;
		if (addr.addresses.size()>0)
			cout << "First address: " << addr.addresses[0]<< endl;
		
		addr = network::getHostInfo("127.0.0.1");
		cout << "Name: " << addr.name << endl;
		if (addr.addresses.size()>0)
			cout << "First address: " << addr.addresses[0]<< endl;

		cout << network::getHostPrimaryIp("google.com") << endl;

		dodoStringArray ifaces = network::getInterfacesNames();
		for (unsigned int i(0); i < ifaces.size(); i++)
		{
			try
			{
				cout << ifaces[i] << ":\t" << network::getInterfaceInfo(ifaces[i]).hwaddr << endl;
				cout << ifaces[i] << ":\t" << network::getInterfaceInfo(ifaces[i]).broadcast << endl;
				cout << ifaces[i] << ":\t" << network::getInterfaceInfo(ifaces[i]).netmask << endl;
			}
			catch (dodo::exception::basic ex)
			{
				cout << ifaces[i] << ":\t" << (std::string)ex << endl;
			}
		}

		cout << network::getInterfaceInfo("lo").hwaddr << endl;

		cout << network::getLocalName() << endl;

		cout << network::getHostInfo("google.com").name << " : " << endl;

		dodoStringArray addresses = network::getHostInfo("google.com").addresses;
		dodoStringArray::iterator i = addresses.begin(), j = addresses.end();
		for (; i != j; ++i)
			cout << *i << endl;

	}
	catch (dodo::exception::basic ex)
	{
		cout << (std::string)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}
