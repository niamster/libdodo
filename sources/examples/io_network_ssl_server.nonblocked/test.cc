/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;

#ifdef OPENSSL_EXT
using namespace io::network::ssl;
#endif

using namespace std;

int
main(int  argc UNUSED,
     char **argv UNUSED)
{
    dodo_try {
#ifdef OPENSSL_EXT
        server s(io::network::connection::PROTOCOL_FAMILY_IPV4, io::network::connection::TRANSFER_STREAM);

        exchange::__init__ accepted;

        s.serve("127.0.0.1", 7778, 1);
        s.setOption(io::network::connection::OPTION_REUSE_ADDRESS, true);
        s.setLingerOption(io::network::connection::LINGER_OPTION_HARD_CLOSE);
        s.blockInherited = true;
        s.block(false);

        io::ssl::__certificates__ certs;
        certs.ca = "host.pem";
        certs.cipher = "RC4-MD5";

        s.setSertificates(certs);

        io::event::manager manager;

        char trimSym[] = {
            '\r', '\n'
        };

        while (true) {
            if (s.accept(accepted)) {
                exchange ex(accepted);

                if (ex.isBlocked())
                    cout << "Blocked" << endl;
                else
                    cout << "Non blocked" << endl;

                int pos = manager.add(ex);

                dodo::string data;

                while (true) {
                    if (manager.isReadable(pos)) {
                        data = ex.readString();
                        cout << data << endl;

                        if (tools::string::trim(data, trimSym, 2) == "*eit") {
                            ex.close();

                            tools::os::die(data);
                        }
                    }
                }
            }
        }
#endif
    } dodo_catch (exception::basic *e)   {
        cout << (dodo::string)*e << "\t" << e->file << "\t" << e->line << endl;
    }

    return 0;
}
