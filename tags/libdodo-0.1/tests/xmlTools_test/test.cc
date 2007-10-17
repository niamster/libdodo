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
		
		//def.name = "test";
		
		//def.chLimit = 1;

		//def.ns = "edi";
		
		def.children.push_back(def);
		
		__node node = tool.parseFile(/*def,*/"./test.xml");
		
		cout << node.attributes["iD"] << endl;
		cout << node.name << endl;
		cout << tools::trim(node.value," \n\t\r",4) << endl;
		
		cout << node.children[0].size() << endl;
		
		//cout << node.children[0][0].attributes.size() << endl;
		
		for (int i (0);i<node.children[0].size();i++)
		{
			//cout << node.children[0][i].attributes["id"] << endl;
			cout << tools::trim(node.children[0][i].value," \n\t\r",4) << "##" << node.children[0][i].children[0].size() << endl;
			for (int j (0);j<node.children[0][i].children[0].size();j++)
				cout << "\t" << tools::trim(node.children[0][i].children[0][j].value," \n\t\r",4) << "@@" << endl;
		}
/*		cout << node.children[0][1].attributes["id"] << endl;
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
