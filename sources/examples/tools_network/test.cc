/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;
using namespace tools;

using namespace std;

int
main(int  argc UNUSED,
     char **argv UNUSED)
{
    try {
        network::__host__ host;

        host = network::host("localhost");
        cout << "Name: " << host.name << endl;
        if (host.addresses.size() > 0)
            cout << "First address: " << host.addresses[0] << endl;

        host = network::host("127.0.0.1");
        cout << "Name: " << host.name << endl;
        if (host.addresses.size() > 0)
            cout << "First address: " << host.addresses[0] << endl;

        cout << network::hostPrimaryIp("google.com") << endl;

        dodoStringArray ifaces = network::interfacesNames();
        for (unsigned int i(0); i < ifaces.size(); i++) {
            try {
                cout << ifaces[i] << ":\t" << network::interface(ifaces[i]).hwaddr << endl;
                cout << ifaces[i] << ":\t" << network::interface(ifaces[i]).broadcast << endl;
                cout << ifaces[i] << ":\t" << network::interface(ifaces[i]).netmask << endl;
            } catch (dodo::exception::basic &ex)   {
                cout << ifaces[i] << ":\t" << (std::string)ex << endl;
            }
        }

        cout << network::interface("lo").hwaddr << endl;

        cout << network::localName() << endl;

        cout << network::host("google.com").name << " : " << endl;

        dodoStringArray addresses = network::host("google.com").addresses;
        dodoStringArray::iterator i = addresses.begin(), j = addresses.end();
        for (; i != j; ++i)
            cout << *i << endl;

        //tools::misc::mail("niam", "test", "test");
        //tools::misc::mail("127.0.0.1", 25, "niam@niam.mu", "niam@niam.mu", "test", "test", "niam", "niam");
    } catch (dodo::exception::basic &ex)   {
        cout << (std::string)ex << "\t" << ex.line << "\t" << ex.file << endl;
    }

    return 0;
}
