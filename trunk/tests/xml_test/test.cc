/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/baseEx.h>
#include <libdodo/xmlProcessor.h>
#include <libdodo/toolsMisc.h>

#include <iostream>

using namespace dodo;
using namespace xml;

using namespace std;


int main(int argc, char **argv)
{
	try
	{
		processor xmlp;
		xmlp.icaseNames = true;

		cout << xmlp.getFileInfo("./test.xml").version;

		__nodeDef def;

		dodoStringArray attr;
		attr.push_back("iD");
		def.attributes = attr;
		def.name = "div";
		//def.chLimit = 1;
		def.ns = "cns";

		__nodeDef def1;
		def1.name = "span";
		def1.ignoreChildrenDef = true;

		def.children["span"] = def1;

		node xnode = xmlp.parseFile(def, "./test.xml");
		//node xnode = xmlp.parseFile("./test.xml");

		cout << xnode.attributes["iD"] << endl;
		cout << xnode.name << endl;
		cout << tools::string::trim(xnode.value, " \n\t\r", 4) << endl;
		cout << xnode.children.size() << endl;

		if (xnode.children.size() > 0)
		{
			cout << xnode.children.begin()->first << endl;
			cout << xnode.children.begin()->second.size() << endl;
		}

		if (xnode.children["span"].size() > 0)
		{
			if (xnode.children["span"][0].children["span"].size() > 0)
				cout << xnode.children["span"][0].children["span"][0].value << endl;

			cout << xnode.children["span"][0].attributes["id"] << endl;

			if (xnode.children["span"].size() > 1)
				cout << xnode.children["span"][1].attributes["id"] << endl;
		}

		xmlp.clear();

		cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;

		cout  << endl << xmlp.make(xmlp.parseFile("./test.xml"), xmlp.getFileInfo("./test.xml").encoding) << endl << endl;

	}
	catch (baseEx ex)
	{
		cout << (string)ex << "\t" << ex.line << endl;
	}
	catch (exception ex)
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
