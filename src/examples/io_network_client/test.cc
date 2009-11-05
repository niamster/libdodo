/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;
using namespace io::network;

using namespace std;

int
main(int  argc,
     char **argv)
{
    try {
        dodoString host = "kernel.org";

        cout << host << ":" << tools::network::hostPrimaryIp(host) << endl;

        client st(io::network::connection::PROTOCOL_FAMILY_IPV4, io::network::connection::TRANSFER_STREAM);

        exchange ex;
        dodoString str;

        st.connect(tools::network::hostPrimaryIp(host), 80, ex);

        str = "GET / HTTP/1.1\r\n";
        ex.bs = str.size();
        ex.write(str);

        str = "Host: " + host + "\r\n";
        ex.bs = str.size();
        ex.write(str);

        str = "Connection: Close\r\n";
        ex.bs = str.size();
        ex.write(str);

        str = "User-Agent: " PACKAGE_NAME "/" PACKAGE_VERSION "\r\n\r\n";
        ex.bs = str.size();
        ex.write(str);

        ex.bs = 512;
        str = ex.readString();

        cout << tools::misc::split(str, "\r\n\r\n")[0] << endl;
    } catch (dodo::exception::basic &ex)   {
        cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
    }

    return 0;
}
