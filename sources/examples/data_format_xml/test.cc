/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;
using namespace data::format::xml;

using namespace std;


int
main(int  argc UNUSED,
     char **argv UNUSED)
{
#ifdef LIBXML2_EXT
    try {
        processor p;
        p.icaseNames = true;

        __definition__ def("div", "cns");
        def.attributes = dodoStringArray(1, "id");
        def.allChildren = false;

        def.children["span"] = __definition__("span");

        node xnode = p.process(def, io::file::regular("./test.xml", io::file::regular::OPEN_MODE_READ_ONLY));
        //node xnode = p.process(io::file::regular("./test.xml", io::file::regular::OPEN_MODE_READ_ONLY));

        cout << p.information().version << endl;

        cout << xnode.attributes["id"] << endl;
        cout << xnode.name << endl;
        cout << tools::string::trim(xnode.value(), " \n\t\r", 4) << endl;

        dodoStringArray names = xnode.childrenNames(true);
        cout << "Names(" << names.size() << "):" << endl;
        dodoStringArray::iterator i = names.begin(), j = names.end();
        for (; i != j; ++i)
            cout << "\t" << *i << endl;

        dodoArray<node> span = xnode.children("span");
        if (span.size() > 0) {
            dodoArray<node> subspan = span[0].children("span");
            if (subspan.size() > 0)
                cout << subspan[0].value() << endl;

            cout << span[0].attributes["id"] << endl;

            if (span.size() > 1)
                cout << span[1]["id"] << endl;
        }

        p.clear();

        cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;

        io::memory buffer;
        p.make(p.process(io::file::regular("./test.xml", io::file::regular::OPEN_MODE_READ_ONLY)), "utf-8", "1.0", buffer);
        cout  << endl << buffer << endl << endl;
    } catch (dodo::exception::basic &ex)   {
        cout << (dodoString)ex << "\t" << ex.line << endl;
    } catch (std::exception &ex)   {
        cout << ex.what();
    } catch (...)   {
        cout << "WTF";
    }
#else
    cout << "No XML extension was compiled!";
#endif

    return 0;
}
