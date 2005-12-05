#include <xmlTools.h>
#include <tools.h>

using namespace dodo;
using namespace std;


int main(int argc, char **argv)
{

	try
	{
		xmlTools tool;
		tool.icase = true;
		
		__nodeDef def;
		
		stringArr attr;
		attr.push_back("iD");
		def.attributes = attr;
		
		def.name = "test";
		
		//def.chLimit = 1;

//		def.ns = "edi";
		
		def.children.push_back(def);
		
		__node node = tool.parseFile(def,"./test.xml");
		
		cout << node.attributes["id"] << endl;
		//cout << node.name << endl;
		//cout << tools::trim(node.value," \n\t\r",4) << endl;
		
		//cout << node.children[0][0].attributes.size() << endl;
		
/*		cout << node.children[0][0].attributes["id"] << endl;
		cout << node.children[0][0].value << endl;
		
		cout << node.children[0][1].attributes["id"] << endl;
		cout << node.children[0][0].ns << endl;*/
	}
	catch(baseEx ex)
	{
		cout << ex << "\t" << ex.line << endl;
	}
	catch(...)
	{
		cout << "WTF";
	}
	return 0;
}
