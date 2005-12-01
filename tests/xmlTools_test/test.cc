#include <xmlTools.h>
#include <tools.h>

using namespace dodo;
using namespace std;


int main(int argc, char **argv)
{

	try
	{
		xmlTools tool;
		
		__nodeDef def;
		
		stringArr attr;
		attr.push_back("id");
		def.attributes = attr;
		
		def.name = "bebe";
		
		def.chLimit = 0;
		
		def.children.push_back(def);
		
		__node node = tool.parseFile(def,"./test.xml");
		
		cout << node.attributes["id"] << endl;
		cout << node.name << endl;
		cout << node.value << endl;
		
		cout << node.children[0][0].attributes["id"] << endl;
		cout << node.children[0][0].value << endl;
		
		cout << node.children[0][1].attributes["id"] << endl;
		cout << node.children[0][1].value << endl;
	}
	catch(baseEx ex)
	{
		cout << ex << "\t" << ex.line << endl;
	}
	return 0;
}
