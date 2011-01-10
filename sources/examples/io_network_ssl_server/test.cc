/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>
#include <stdlib.h>

using namespace dodo;
using namespace std;

#ifdef OPENSSL_EXT
using namespace io::network::ssl;

#ifndef IO_WO_XEXEC
void
hook(xexec::__collected_data__ *odata,
     short                     type UNUSED,
     short                     operation UNUSED,
     void                      *udata UNUSED)
{
    io::channel::__collected_data__ *st = (io::channel::__collected_data__ *)odata;

    cout << "In hook: " << st->buffer << endl;
    cout << (dynamic_cast<exchange *>(st->executor)->isAlive() ? "Alive" : "Dead") << endl;
}
#endif

void
process(exchange &ex)
{
#ifndef IO_WO_XEXEC
    ex.addXExec(xexec::ACTION_PREEXEC, ::hook, NULL);
    ex.addXExec(xexec::ACTION_POSTEXEC, ::hook, NULL);
#endif

    ex.setInBufferSize(1);
    ex.setOutBufferSize(1);

    ex.bs = 7;
    ex.writeString("test\n");

    if (ex.isAlive())
        ex.writeString("Alive");

    dodo::string str = "";
    dodo_try {
        ex.bs = 4;
        str = ex.read();

        cout << str << ":" << str.size() << endl;
        cout.flush();
        if (str == "*eit") {
            ex.close();

            exit(0);
        }
    } dodo_catch (dodo::exception::basic *e)   {
        cout << (dodo::string)*e << "\t" << e->line << "\t" << e->file << endl;
        cout.flush();
    }
}
#endif

int
main(int  argc UNUSED,
     char **argv UNUSED)
{
    dodo_try {
#ifdef OPENSSL_EXT
        server sock(io::network::connection::PROTOCOL_FAMILY_IPV4, io::network::connection::TRANSFER_STREAM);

        io::network::connection::__peer__ info;
        io::network::ssl::exchange::__init__ accepted;

        sock.serve("127.0.0.1", 7778, 3);

        io::ssl::__certificates__ certs;
        certs.ca = "host.pem";
        certs.cipher = "RC4-MD5";

        sock.setSertificates(certs);

        while (true) {
            if (sock.accept(accepted, info)) {
                if (sock.isBlocked()) {
                    cout << "Blocked\n";
                    cout.flush();
                }

                exchange ex(accepted);

                process(ex);
            }
        }
#endif
    } dodo_catch (dodo::exception::basic *e)   {
        cout << (dodo::string)*e << "\t" << e->line << "\t" << e->file << endl;
    }

    return 0;
}
