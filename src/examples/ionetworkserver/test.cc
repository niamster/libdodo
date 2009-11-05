/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>
#include <stdlib.h>

using namespace dodo;
using namespace io::network;

using namespace std;

void
process(exchange &ex)
{
    if (ex.isBlocked()) {
        cout << "Blocked\n";
        cout.flush();
    } else   {
        cout << "Non blocked\n";
        cout.flush();
    }

    ex.setInBufferSize(1);
    ex.setOutBufferSize(1);

    ex.bs = 7;
    ex.writeString("test\n");

    dodoString str = "";
    try {
        ex.bs = 4;
        str = ex.read();

        cout << str << ":" << str.size() << endl;
        cout.flush();
        if (str.compare("exit") == 0) {
            ex.close();

            exit(0);
        }
    } catch (dodo::exception::basic &ex)   {
        cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
        cout.flush();
    }
}

int
main(int  argc,
     char **argv)
{
    try {
        server s(connection::PROTOCOL_FAMILY_IPV4, connection::TRANSFER_STREAM);
        //server s(connection::PROTOCOL_FAMILY_IPV6, connection::TRANSFER_STREAM);
        //server s(connection::PROTOCOL_FAMILY_UNIX_SOCKET, connection::TRANSFER_STREAM);

        exchange::__peer__ info;
        exchange::__init__ accepted;

        s.serve("127.0.0.1", 7778, 3);
        //s.serve("::",7777);
        //s.serve("./unix.socket",10,true);

        s.blockInherited = false;
        s.block(false);

        s.setLingerOption(exchange::LINGER_OPTION_HARD_CLOSE);

        while (true) {
            if (s.accept(accepted, info)) {
                if (s.isBlocked()) {
                    cout << "Blocked\n";
                    cout.flush();
                }

                exchange ex(accepted);

                process(ex);
            }
        }
    } catch (dodo::exception::basic &ex)   {
        cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
    }

    return 0;
}
