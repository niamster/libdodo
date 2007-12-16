#include <libdodo/tools.h>
#include <libdodo/json.h>

#include <iostream>

using namespace dodo;
using namespace std;

int main(int argc, char **argv)
{	
	try
	{
		json js;

		jsonNodeDef node, node1, node2;
		
		node.valueDataType = JSON_DATATYPE_OBJECT;
		
		node2.valueDataType = JSON_DATATYPE_ARRAY;

		node1.valueDataType = JSON_DATATYPE_BOOLEAN;
		node1.booleanValue = true;
		node.objectValue["bool"] = node1;
		node2.arrayValue.push_back(node1);
		
		node1.valueDataType = JSON_DATATYPE_STRING;
		node1.stringValue = "test string";
		node.objectValue["string"] = node1;
		node2.arrayValue.push_back(node1);

		node1.valueDataType = JSON_DATATYPE_NUMERIC;
		node1.numericValue = 1234;
		node.objectValue["numeric"] = node1;
		node2.arrayValue.push_back(node1);

		node1.valueDataType = JSON_DATATYPE_NULL;
		node.objectValue["null"] = node1;
		node2.arrayValue.push_back(node1);

		node.objectValue["object"] = node2;


		cout << js.makeJSON(node) << endl;
		
		jsonNode jsN;
		cout << "string: " << "as\\\"sdq\\\\weq" << endl;
		js.processJSON(jsN, "\"as\\\"sdq\\\\weq\"");
		
		cout << jsN.getType() << endl;
		
		switch (jsN.getType())
		{
			case JSON_DATATYPE_STRING:
				
				cout << jsN.getString() << endl;
				
				break;
		}
	}
	catch(baseEx ex)
	{
    		cout << endl << ex.baseErrstr << endl << ex.line << "!!" << ex.baseErrno << "!!" << endl;
    }
    
	return 0;
}

