#include <libdodo/baseEx.h>
#include <libdodo/xml.h>
#include <libdodo/tools.h>

#include <iostream>

using namespace dodo;
using namespace std;


int main(int argc, char **argv)
{
#ifdef LIBXML2_EXT
		
		try
		{
			xml tool;
			tool.icaseNames = true;
			
			cout << tool.getXMLFileInfo("./test.xml").version;
			
			__xmlNodeDef def;
			
			dodoStringArray attr;
			attr.push_back("iD");
			def.attributes = attr;	
			def.name = "div";	
			//def.chLimit = 1;
			def.ns = "cns";
			
			__xmlNodeDef def1;
			def1.name = "span";
			def1.ignoreChildrenDef = true;
			
			def.children["span"] = def1;
			
			__xmlNode node = tool.parseFile(def,"./test.xml");
			//__xmlNode node = tool.parseFile("./test.xml");
					
			cout << node.attributes["iD"] << endl;
			cout << node.name << endl;
			cout << stringTools::trim(node.value," \n\t\r", 4) << endl;
			cout << node.children.size() << endl;
			
			if (node.children.size()>0)
			{
				cout << node.children.begin()->first << endl;
				cout << node.children.begin()->second.size() << endl;
			}
			
			if (node.children["span"].size() > 0)
			{
				if (node.children["span"][0].children["span"].size() > 0)
					cout << node.children["span"][0].children["span"][0].value << endl;
				
				cout << node.children["span"][0].attributes["id"] << endl;
				
				if (node.children["span"].size() > 1)
					cout << node.children["span"][1].attributes["id"] << endl;
			}
			
			tool.clear();
			
			cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
			
			cout  << endl << tool.createXML(tool.parseFile("./test.xml")) << endl << endl;
			
		}
		catch(baseEx ex)
		{
			cout << (string)ex << "\t" << ex.line << endl;
		}
		catch(exception ex)
		{
			cout << ex.what();
		}
		catch(...)
		{
			cout << "WTF";
		}
	
#else
	
		cout << "No XML extension was compiled!";
	
#endif
	
	return 0;
}
