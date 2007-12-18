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
		js.processJSON(jsN, js.makeJSON(node));
		
		switch (jsN.getType())
		{		
			case JSON_DATATYPE_OBJECT:

				dodoMap<dodoString, jsonNode, stringTools::equal> objectValue = jsN.getObject();
				cout << "size: " << objectValue.size() << endl;
				std::map<dodoString, jsonNode>::iterator i = objectValue.begin(), j = objectValue.end();
				
				for (;i!=j;++i)
				{
					cout << i->first << ": ";
					switch (i->second.getType())
					{
						case JSON_DATATYPE_STRING:
							
							cout << "[string]: " << i->second.getString() << endl;
							
							break;
							
						case JSON_DATATYPE_OBJECT:
							
							cout << "[object]: " << endl;
							
							break;
							
						case JSON_DATATYPE_NULL:
							
							cout << "[null]: " << endl;
							
							break;
							
						case JSON_DATATYPE_NUMERIC:
							
							cout << "[numeric]: " << endl;
							
							break;
							
						case JSON_DATATYPE_BOOLEAN:
							
							cout << "[boolean]: " << endl;
							
							break;
							
						case JSON_DATATYPE_ARRAY:
						{
							dodoArray<jsonNode> objectValue = i->second.getArray();
							dodoArray<jsonNode>::iterator o = objectValue.begin(), p = objectValue.end();
							
							cout << "[array]: " << endl;
							for (;o!=p;++o)
								switch (o->getType())
								{
									case JSON_DATATYPE_STRING:
										
										cout << "\t[string]: " << o->getString() << endl;
										
										break;
										
									case JSON_DATATYPE_ARRAY:
										
										cout << "\t[array]: " << endl;
										
										break;
										
									case JSON_DATATYPE_OBJECT:
										
										cout << "\t[object]: " << endl;
										
										break;
										
									case JSON_DATATYPE_NULL:
										
										cout << "\t[null]: " << endl;
										
										break;
										
									case JSON_DATATYPE_NUMERIC:
										
										cout << "\t[numeric]: " << endl;
										
										break;
										
									case JSON_DATATYPE_BOOLEAN:
										
										cout << "\t[boolean]: " << endl;
										
										break;
								}
							
							break;
						}
					}
				}
				
				break;
		}
	}
	catch(baseEx ex)
	{
    		cout << endl << ex.baseErrstr << endl << ex.line << "!!" << ex.baseErrno << "!!" << endl;
    }
    
	return 0;
}

