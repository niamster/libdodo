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
		//def.ns = "edi";
		
		__nodeDef def1;
		def1.name = "bebe";
		
		def1.children.push_back(def1);
		
		def.children.push_back(def1);
		
		__node node = tool.parseFile(def,"./test.xml");
		
		cout << node.attributes["iD"] << endl;
		cout << node.name << endl;
		cout << tools::trim(node.value," \n\t\r",4) << endl;
		
		cout << node.children[0].size() << endl;
		
		cout << node.children[0][0].attributes.size() << endl;
		
		for (int i (0);i<node.children[0].size();i++)
		{
			cout << node.children[0][i].children[0][0].attributes["id"] << endl;
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
