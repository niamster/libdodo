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

		jsonNode node, node1, node2;
		
		node.setType(JSON_DATATYPE_OBJECT);
		
		node2.setType(JSON_DATATYPE_ARRAY);

		node1.setType(JSON_DATATYPE_BOOLEAN);
		node1.setBoolean(true);
		node.addObjectMember("bool", node1);
		node2.addArrayElement(node1);
		
		node1.setType(JSON_DATATYPE_STRING);
		node1.setString("test string");
		node.addObjectMember("string", node1);
		node2.addArrayElement(node1);

		node1.setType(JSON_DATATYPE_NUMERIC);
		node1.setNumeric(1234);
		node.addObjectMember("numeric", node1);
		node2.addArrayElement(node1);

		node1.setType(JSON_DATATYPE_NULL);
		node.addObjectMember("null", node1);
		node2.addArrayElement(node1);

		node.addObjectMember("big numeric", (long)1234567890);
		node2.addArrayElement(node);

		node1.setType(JSON_DATATYPE_OBJECT);
		node1.addObjectMember("string", dodoString("string in object"));
		node.addObjectMember("null", node1);
		node2.addArrayElement(node1);

		node.addObjectMember("object", node2);

		cout << js.makeJSON(node) << endl;
		
		jsonNode jsN = js.processJSON(js.makeJSON(node));
	
		switch (jsN.getType())
		{		
			case JSON_DATATYPE_OBJECT:

				dodoMap<dodoString, jsonNode, dodoMapStringCompare> objectValue = jsN.getObject();
				cout << "size: " << objectValue.size() << endl;
				dodoMap<dodoString, jsonNode>::iterator i = objectValue.begin(), j = objectValue.end();
				
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
							
							cout << "[null]: null" << endl;
							
							break;
							
						case JSON_DATATYPE_NUMERIC:
							
							cout << "[numeric]: " << i->second.getNumeric() << endl;
							
							break;
							
						case JSON_DATATYPE_BOOLEAN:
							
							cout << "[boolean]: " << (i->second.getBoolean()?"true":"false") << endl;
							
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
										
										cout << "\t[null]: null" << endl;
										
										break;
										
									case JSON_DATATYPE_NUMERIC:
										
										cout << "\t[numeric]: " << o->getNumeric() << endl;
										
										break;
										
									case JSON_DATATYPE_BOOLEAN:
										
										cout << "\t[boolean]: " << (o->getBoolean()?"true":"false") << endl;
										
										break;
								}
							
							break;
						}
					}
				}
				
				break;
		}
		
		dodoStringMap map;
		map["test"] = "test";
		map["foo"] = "bar";
		map["one"] = "two";
		
		cout << endl << js.mapToJSON(map) << endl;
	}
	catch(baseEx ex)
	{
    		cout << endl << ex.baseErrstr << endl << ex.line << "!!" << ex.baseErrno << "!!" << endl;
    }
    
	return 0;
}

