#include <libdodo/tools.h>
#include <libdodo/jsonProcessor.h>
#include <libdodo/jsonNode.h>

#include <iostream>

using namespace dodo;
using namespace json;

using namespace std;

int main(int argc, char **argv)
{	
	try
	{
		processor js;

		node node0, node1, node2;
		
		node1.setBoolean(true);
		node0.addObjectMember("bool", node1);
		node2.addArrayElement(node1);
		
		node1.setString("test string");
		node0.addObjectMember("string", node1);
		node2.addArrayElement(node1);

		node1.setNumeric(1234);
		node0.addObjectMember("numeric", node1);
		node2.addArrayElement(node1);

		node1.setNull();
		node0.addObjectMember("null", node1);
		node2.addArrayElement(node1);

		node0.addObjectMember("big numeric", (long)1234567890);
		node2.addArrayElement(node0);

		node1.addObjectMember("string", dodoString("string in object"));
		node0.addObjectMember("null", node1);
		node2.addArrayElement(node1);

		node0.addObjectMember("object", node2);

		cout << js.make(node0) << endl;
		
		node jsN = js.process(js.make(node0));
	
		switch (jsN.getType())
		{		
			case NODE_DATATYPE_OBJECT:

				dodoMap<dodoString, node, dodoMapStringCompare> objectValue = jsN.getObject();
				cout << "size: " << objectValue.size() << endl;
				dodoMap<dodoString, node>::iterator i = objectValue.begin(), j = objectValue.end();
				
				for (;i!=j;++i)
				{
					cout << i->first << ": ";
					switch (i->second.getType())
					{
						case NODE_DATATYPE_STRING:
							
							cout << "[string]: " << i->second.getString() << endl;
							
							break;
							
						case NODE_DATATYPE_OBJECT:
							
							cout << "[object]: " << endl;
							
							break;
							
						case NODE_DATATYPE_NULL:
							
							cout << "[null]: null" << endl;
							
							break;
							
						case NODE_DATATYPE_NUMERIC:
							
							cout << "[numeric]: " << i->second.getNumeric() << endl;
							
							break;
							
						case NODE_DATATYPE_BOOLEAN:
							
							cout << "[boolean]: " << (i->second.getBoolean()?"true":"false") << endl;
							
							break;
							
						case NODE_DATATYPE_ARRAY:
						{
							dodoArray<node> objectValue = i->second.getArray();
							dodoArray<node>::iterator o = objectValue.begin(), p = objectValue.end();
							
							cout << "[array]: " << endl;
							for (;o!=p;++o)
								switch (o->getType())
								{
									case NODE_DATATYPE_STRING:
										
										cout << "\t[string]: " << o->getString() << endl;
										
										break;
										
									case NODE_DATATYPE_ARRAY:
										
										cout << "\t[array]: " << endl;
										
										break;
										
									case NODE_DATATYPE_OBJECT:
										
										cout << "\t[object]: " << endl;
										
										break;
										
									case NODE_DATATYPE_NULL:
										
										cout << "\t[null]: null" << endl;
										
										break;
										
									case NODE_DATATYPE_NUMERIC:
										
										cout << "\t[numeric]: " << o->getNumeric() << endl;
										
										break;
										
									case NODE_DATATYPE_BOOLEAN:
										
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
		
		cout << endl << js.fromMap(map) << endl;
	}
	catch(baseEx ex)
	{
    		cout << endl << ex.baseErrstr << endl << ex.line << "!!" << ex.baseErrno << "!!" << endl;
    }
    
	return 0;
}

