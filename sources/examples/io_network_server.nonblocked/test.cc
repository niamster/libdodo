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
main(int  argc UNUSED,
     char **argv UNUSED)
{
    dodo_try {
        server s(connection::PROTOCOL_FAMILY_IPV4, connection::TRANSFER_STREAM);

        exchange::__init__ accepted;

        s.serve("127.0.0.1", 7778, 1);
        s.setOption(connection::OPTION_REUSE_ADDRESS, true);
        s.setLingerOption(connection::LINGER_OPTION_HARD_CLOSE);
        s.blockInherited = true;
        s.block(false);

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
                        cout << "'" << tools::string::trim(data, trimSym, 2) << "'" << endl;

                        if (tools::string::trim(data, trimSym, 2) == "*eit")
                            tools::os::die(data);
                    }
                }
            }
        }
    } dodo_catch (dodo::exception::basic *e)   {
        cout << (dodo::string)*e << "\t" << e->file << "\t" << e->line << endl;
    }

    return 0;
}
