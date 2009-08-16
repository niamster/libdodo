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


int main(int argc, char **argv)
{
#ifdef LIBXML2_EXT
	try
	{
		processor xmlp;
		xmlp.icaseNames = true;

		__nodeDef__ def("div", "cns");
		def.attributes = dodoStringArray(1, "id");
		def.allChildren = false;

		def.children["span"] = __nodeDef__("span");

		node xnode = xmlp.process(def, io::file::regular("./test.xml", io::file::REGULAR_OPENMODE_READ_ONLY));
		//node xnode = xmlp.process(io::file::regular("./test.xml", io::file::REGULAR_OPENMODE_READ_ONLY));

		cout << xmlp.getInfo().version << endl;

		cout << xnode.attributes["id"] << endl;
		cout << xnode.name << endl;
		cout << tools::string::trim(xnode.getValue(), " \n\t\r", 4) << endl;

		dodoStringArray names = xnode.getChildrenNames(true);
		cout << "Names(" << names.size() << "):" << endl;
		dodoStringArray::iterator i = names.begin(), j = names.end();
		for (;i!=j;++i)
			cout << "\t" << *i << endl;

		dodoArray<node> span = xnode.getChildren("span");

		if (span.size() > 0)
		{
			dodoArray<node> subspan = span[0].getChildren("span");
			if (subspan.size() > 0)
				cout << subspan[0].getValue() << endl;

			cout << span[0].attributes["id"] << endl;

			if (span.size() > 1)
				cout << span[1]["id"] << endl;
		}

		xmlp.clear();

		cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;

		io::memory buffer;
		xmlp.make(xmlp.process(io::file::regular("./test.xml", io::file::REGULAR_OPENMODE_READ_ONLY)), "utf-8", "1.0", buffer);
		cout  << endl << buffer << endl << endl;

	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << endl;
	}
	catch (std::exception &ex)
	{
		cout << ex.what();
	}
	catch (...)
	{
		cout << "WTF";
	}
#else
	cout << "No XML extension was compiled!";
#endif

	return 0;
}
