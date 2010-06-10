/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;
using namespace io;

using namespace std;

#ifndef IO_WO_XEXEC
void
hook(xexec::__collected_data__ *odata,
     short                     type UNUSED,
     short                     operation,
     void                      *udata UNUSED)
{
    io::channel::__collected_data__ *st = (io::channel::__collected_data__ *)odata;
    if (operation == io::channel::OPERATION_WRITE) {
        stdio *io = dynamic_cast<stdio *>(st->executor);
        dodo::string buffer = st->buffer;
        io->bs = 100;
        io->writeString("\nhook\n");

        dodo::string str = ">" + buffer + "<\n";

        io->bs = str.size();
        st->buffer.assign(str);
    }
}
#endif

int
main(int  argc UNUSED,
     char **argv UNUSED)
{
    try {
        stdio st;
        cout << st.peer().host << endl;

#ifndef IO_WO_XEXEC
        st.addXExec(xexec::ACTION_PREEXEC, ::hook, NULL);
#endif

        st.bs = sizeof("write");
        st.write("write");
        st.flush();

#ifndef IO_WO_XEXEC
        st.disableXExecs = true;
#endif
        dodo::string o;

        st.bs = 33;

        o = st.read();

        cout << o.size() << "\n";
        cout << o << "\n";

        st.bs = 4;
        st.write("1234567890");
        st.writeString("\n");

        st.bs = 40;
        st.writeString(o);
        st.writeString("\nexiting\n");
    } catch (dodo::exception::basic &ex)   {
        cout << (dodo::string)ex << "\t" << ex.line << "\t" << ex.file << endl;
    }

    return 0;
}
