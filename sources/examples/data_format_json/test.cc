/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;
using namespace data::format::json;

using namespace std;

void
showJSON(const node &n,
         short      level = 0)
{
    switch (n.type()) {
        case node::DATA_STRING:

            cout << dodo::string(level, '\t') << "[string]: " << n.string() << endl;

            break;

        case node::DATA_OBJECT:
        {
            cout << dodo::string(level, '\t') << "[object]: " << endl;
            const dodoMap<dodo::string, node, dodoMapStringCompare>
            &objectValue = n.object();
            dodoMap<dodo::string, node, dodoMapStringCompare>::const_iterator
            i = objectValue.begin(),
            j = objectValue.end();
            for (; i != j; ++i) {
                cout << dodo::string(level + 1, '\t') << "'" << i->first << "'" << endl;
                showJSON(i->second, level + 2);
            }

            break;
        }

        case node::DATA_NULL:

            cout << dodo::string(level, '\t') << "[null]" << endl;

            break;

        case node::DATA_NUMERIC:

            cout << dodo::string(level, '\t') << "[numeric]: " << n.numeric() << endl;

            break;

        case node::DATA_BOOLEAN:

            cout << dodo::string(level, '\t') << "[boolean]: " << (n.boolean() ? "true" : "false") << endl;

            break;

        case node::DATA_ARRAY:
        {
            const dodoArray<node> &arrayValue = n.array();
            dodoArray<node>::const_iterator i = arrayValue.begin(), j = arrayValue.end();

            cout << dodo::string(level, '\t') << "[array]: " << endl;
            for (; i != j; ++i)
                showJSON(*i, level + 1);

            break;
        }
    }
}

int
main(int  argc UNUSED,
     char **argv UNUSED)
{
    try {
        processor p;

        node node0, node1, node2;

        node1.setBoolean(true);
        node0.addObjectMember("bool", node1);
        node2.addArrayElement(node1);

        node1.setString("test string");
        node0.addObjectMember("string", node1);
        node2.addArrayElement(node1);

        node1.setNumeric(1234);
        node0.addObjectMember("numeric", node1);
        node2.addArrayElement(node1);

        node1.setNull();
        node0.addObjectMember("null", node1);
        node2.addArrayElement(node1);

        node0.addObjectMember("big numeric", (long)1234567890);
        node2.addArrayElement(node0);

        node1.addObjectMember("string", dodo::string("string in object"));
        node0.addObjectMember("null", node1);
        node2.addArrayElement(node1);

        node0.addObjectMember("object", node2);

        io::memory json;

        p.make(node0, json);
        cout << json << endl;

        json.pos = 0;
        node jn = p.process(json);
        showJSON(jn);

        dodoStringMap map;
        map["test"] = "test";
        map["foo"] = "bar";
        map["one"] = "two";

        json.clear();
        p.fromMap(map, json);
        cout << endl << json << endl;
    } catch (dodo::exception::basic &ex)   {
        cout << (dodo::string)ex << "\t" << ex.line << "\t" << ex.file << endl;
    }

    return 0;
}

