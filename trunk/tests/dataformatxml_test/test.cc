/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/exceptionBasic.h>
#include <libdodo/dataFormatXmlProcessor.h>
#include <libdodo/toolsMisc.h>

#include <iostream>

using namespace dodo;
using namespace data::format::xml;

using namespace std;


int main(int argc, char **argv)
{
	try
	{
		processor xmlp;
		xmlp.icaseNames = true;

		cout << xmlp.getFileInfo("./test.xml").version << endl;

		__nodeDef def;

		dodoStringArray attr;
		attr.push_back("id");
		def.attributes = attr;
		def.name = "div";
		def.ns = "cns";

		__nodeDef def1;
		def1.name = "span";
		def1.allChildren = true;
		def.children["span"] = def1;

		node xnode = xmlp.processFile(def, "./test.xml");
		//node xnode = xmlp.processFile("./test.xml");

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

		cout  << endl << xmlp.make(xmlp.processFile("./test.xml"), xmlp.getFileInfo("./test.xml").encoding) << endl << endl;

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

#ifndef LIBXML2_EXT

	cout << "No XML extension was compiled!";

#endif

	return 0;
}
