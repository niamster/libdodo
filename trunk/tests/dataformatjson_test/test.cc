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

void showJSON(const node &jsN, short level=0)
{
	switch (jsN.getType())
	{
		case DATATYPE_STRING:

			cout << dodoString(level, '\t') << "[string]: " << jsN.getString() << endl;

			break;

		case DATATYPE_OBJECT:
		{
			cout << dodoString(level, '\t') << "[object]: " << endl;
			const dodoMap<dodoString, node, dodoMapStringCompare>
				&objectValue = jsN.getObject();
			dodoMap<dodoString, node, dodoMapStringCompare>::const_iterator
				i = objectValue.begin(),
				j = objectValue.end();
			for (; i != j; ++i)
			{
				cout << dodoString(level+1, '\t') << "'" << i->first << "'" << endl;
				showJSON(i->second, level+2);
			}

			break;
		}

		case DATATYPE_NULL:

			cout << dodoString(level, '\t') << "[null]" << endl;

			break;

		case DATATYPE_NUMERIC:

			cout << dodoString(level, '\t') << "[numeric]: " << jsN.getNumeric() << endl;

			break;

		case DATATYPE_BOOLEAN:

			cout << dodoString(level, '\t') << "[boolean]: " << (jsN.getBoolean() ? "true" : "false") << endl;

			break;

		case DATATYPE_ARRAY:
		{
			const dodoArray<node> &arrayValue = jsN.getArray();
			dodoArray<node>::const_iterator i = arrayValue.begin(), j = arrayValue.end();

			cout << dodoString(level, '\t') << "[array]: " << endl;
			for (; i != j; ++i)
				showJSON(*i, level+1);

			break;
		}
	}
}

int main(int argc, char **argv)
{
	try
	{
		processor js;

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

		node1.addObjectMember("string", dodoString("string in object"));
		node0.addObjectMember("null", node1);
		node2.addArrayElement(node1);

		node0.addObjectMember("object", node2);

		io::memory json;

		js.make(node0, json);
		cout << json << endl;

		json.pos = 0;
		node jn = js.process(json);
		showJSON(jn);

		dodoStringMap map;
		map["test"] = "test";
		map["foo"] = "bar";
		map["one"] = "two";

		json.clear();
		js.fromMap(map, json);
		cout << endl << json << endl;
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}

