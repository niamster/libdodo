#include <xmlTools.h>
#include <tools.h>

using namespace dodo;
using namespace std;


int main(int argc, char **argv)
{

	try
	{
		xmlTools tool;
		tool.icaseNames = true;
		
		cout << tool.getXMLFileInfo("./test.xml").version;
		
		__nodeDef def;
		
		stringArr attr;
		attr.push_back("iD");
		def.attributes = attr;	
		def.name = "hehe";	
		//def.chLimit = 1;
		def.ns = "edi";
		
		__nodeDef def1;
		def1.name = "bebe";
		def1.ignoreChildrenDef = true;
		
		def.children["bebe"] = def1;
		
		__node node = tool.parseFile(def,"./test.xml");
		//__node node = tool.parseFile("./test.xml");
				
		cout << node.attributes["iD"] << endl;
		cout << node.name << endl;
		cout << tools::trim(node.value," \n\t\r",4) << endl;
		cout << node.children.size() << endl;
		
		if (node.children.size()>0)
		{
			cout << node.children.realArr.begin()->first << endl;
			cout << node.children.realArr.begin()->second.size() << endl;
		}
		
		if (node.children["bebe"].size() > 0)
		{
			if (node.children["bebe"][0].children["bebe"].size() > 0)
				cout << node.children["bebe"][0].children["bebe"][0].value << endl;
			
			cout << node.children["bebe"][0].attributes["id"] << endl;
			
			if (node.children["bebe"].size() > 1)
				cout << node.children["bebe"][1].attributes["id"] << endl;
		}
	}
	catch(baseEx ex)
	{
		cout << ex << "\t" << ex.line << endl;
	}
	catch(exception ex)
	{
		cout << ex.what();
	}
	catch(...)
	{
		cout << "WTF";
	}
	return 0;
}
