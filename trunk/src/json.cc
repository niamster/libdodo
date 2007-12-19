/***************************************************************************
 *            json.cc
 *
 *  Mon Oct 20 02:15:55 2007
 *  Copyright  2007  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License version 2.1 as published by
 *  the Free Software Foundation;
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <libdodo/json.h>

using namespace dodo;

jsonNode::jsonNode() : valueDataType(JSON_DATATYPE_NULL)
{
	
}

//-------------------------------------------------------------------

jsonNode::jsonNode(const jsonNode &node)
{
	stringValue = node.stringValue;
	objectValue = node.objectValue;
	arrayValue = node.arrayValue;
	booleanValue = node.booleanValue;
	numericValue = node.numericValue;
	
	valueDataType = node.valueDataType;
}

//-------------------------------------------------------------------

jsonNode::~jsonNode()
{
	
}

//-------------------------------------------------------------------

jsonNode
jsonNode::operator[](const dodoString &key)
{
	if (valueDataType != JSON_DATATYPE_OBJECT)
		throw baseEx(ERRMODULE_JSON, JSONNODEEX_BROPERATORSTRING, ERR_LIBDODO, JSONNODEEX_WRONGTYPEREQUESTED, JSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);
	
	return objectValue[key];
}

//-------------------------------------------------------------------

jsonNode
jsonNode::operator[](unsigned long key)
{
	if (valueDataType != JSON_DATATYPE_ARRAY)
		throw baseEx(ERRMODULE_JSON, JSONNODEEX_BROPERATORNUMERIC, ERR_LIBDODO, JSONNODEEX_WRONGTYPEREQUESTED, JSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);
	
	if (key >= arrayValue.size())
		throw baseEx(ERRMODULE_JSON, JSONNODEEX_BROPERATORNUMERIC, ERR_LIBDODO, JSONNODEEX_ARRAYOUTOFRANGE, JSONNODEEX_ARRAYOUTOFRANGE_STR, __LINE__, __FILE__);
	
	return arrayValue[key];
}

//-------------------------------------------------------------------

short 
jsonNode::getType()
{
	return valueDataType;
}

//-------------------------------------------------------------------

dodoString 
jsonNode::getString()
{
	if (valueDataType != JSON_DATATYPE_STRING)
		throw baseEx(ERRMODULE_JSON, JSONNODEEX_GETSTRING, ERR_LIBDODO, JSONNODEEX_WRONGTYPEREQUESTED, JSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);
	
	return stringValue;
}

//-------------------------------------------------------------------

bool 
jsonNode::getBoolean()
{
	if (valueDataType != JSON_DATATYPE_BOOLEAN)
		throw baseEx(ERRMODULE_JSON, JSONNODEEX_GETBOOLEAN, ERR_LIBDODO, JSONNODEEX_WRONGTYPEREQUESTED, JSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);
	
	return booleanValue;
}

//-------------------------------------------------------------------

long
jsonNode::getNumeric()
{
	if (valueDataType != JSON_DATATYPE_NUMERIC)
		throw baseEx(ERRMODULE_JSON, JSONNODEEX_NUMERIC, ERR_LIBDODO, JSONNODEEX_WRONGTYPEREQUESTED, JSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);
	
	return numericValue;
}


//-------------------------------------------------------------------

dodoArray<jsonNode> 
jsonNode::getArray()
{
	if (valueDataType != JSON_DATATYPE_ARRAY)
		throw baseEx(ERRMODULE_JSON, JSONNODEEX_GETARRAY, ERR_LIBDODO, JSONNODEEX_WRONGTYPEREQUESTED, JSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);
	
	return arrayValue;
}

//-------------------------------------------------------------------

dodoMap<dodoString, jsonNode, stringTools::equal> 
jsonNode::getObject()
{
	if (valueDataType != JSON_DATATYPE_OBJECT)
		throw baseEx(ERRMODULE_JSON, JSONNODEEX_GETOBJECT, ERR_LIBDODO, JSONNODEEX_WRONGTYPEREQUESTED, JSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);
	
	return objectValue;
}

//-------------------------------------------------------------------

bool 
jsonNode::isNull()
{
	return (valueDataType == JSON_DATATYPE_NULL);
}

//-------------------------------------------------------------------

void 
jsonNode::clear()
{
	stringValue.clear();
	objectValue.clear();
	arrayValue.clear();
	
	valueDataType = JSON_DATATYPE_NULL;
}

//-------------------------------------------------------------------

json::json(json &js)
{
	
}

//-------------------------------------------------------------------

json::json()
{
}

//-------------------------------------------------------------------

json::~json()
{
}

//-------------------------------------------------------------------

dodoString 
json::makeJSON(const jsonNodeDef &root)
{
	switch (root.valueDataType)
	{
		case JSON_DATATYPE_STRING:
			{
	
				dodoString jsonObject = "\"";
				jsonObject.append(root.stringValue);
				jsonObject.append("\"");
				
				return jsonObject;
			}
			
		case JSON_DATATYPE_OBJECT:
			{
				dodoString jsonObject = "{";
				
				std::map<dodoString, jsonNodeDef>::const_iterator i = root.objectValue.begin(), j = root.objectValue.end();
				if (i != j)
				{
					for (--j;i!=j;++i)
					{
						jsonObject.append("\"");
						jsonObject.append(i->first);
						jsonObject.append("\":");
						
						jsonObject.append(makeJSON(i->second));
						jsonObject.append(",");
					}
					jsonObject.append("\"");
					jsonObject.append(i->first);
					jsonObject.append("\":");
		
					jsonObject.append(makeJSON(i->second));
				}
	
				jsonObject.append("}");
				
				return jsonObject;
			}
			
		case JSON_DATATYPE_ARRAY:
			{
				dodoString jsonObject = "[";
				
				dodoArray<jsonNodeDef>::const_iterator i = root.arrayValue.begin(), j = root.arrayValue.end() - 1;
				if (i != j)
				{
					for (;i!=j;++i)
					{
						jsonObject.append(makeJSON(*i));
						jsonObject.append(",");
					}
					jsonObject.append(makeJSON(*i));
				}
				
				jsonObject.append("]");
				
				return jsonObject;
			}
			
		case JSON_DATATYPE_NUMERIC:
			
			return stringTools::lToString(root.numericValue);
			
		case JSON_DATATYPE_BOOLEAN:
			
			return root.booleanValue?"true":"false";
			
		case JSON_DATATYPE_NULL:
		default:
			
			return "null";
	}

	return "null";
}

//-------------------------------------------------------------------

unsigned long 
json::processArray(dodoArray<jsonNode> &node, 
					const dodoString &root, 
					unsigned long pos)
{
	node.clear();
	
	short state = JSON_STATE_ARRAY_INITIAL;
	
	jsonNode subNode;
	
	unsigned long i(pos), j(root.size());
	for (;i<j;++i)
	{
		switch (root[i])
		{
			case ' ':
			case '\r':
			case '\n':
			case '\t':
				
				break;
				
			case '[':

				if (state == JSON_STATE_ARRAY_INITIAL)
					state = JSON_STATE_ARRAY_ARRAY;
				else
				{
					i = processValue(subNode, root, i);
					
					node.push_back(subNode);
				}
				
				break;
				
			case ']':
				
				return i;
				
			case ',':
				
				break;
				
			default:

				i = processValue(subNode, root, i);
				
				node.push_back(subNode);
		}
	}
	
	return i;
}

//-------------------------------------------------------------------

unsigned long 
json::processValue(jsonNode &node, 
					const dodoString &root, 
					unsigned long pos)
{	
	node.clear();
	
	unsigned long i(pos), j(root.size());
	for (;i<j;++i)
	{
		switch (root[i])
		{
			case '"':

				node.valueDataType = JSON_DATATYPE_STRING;
				return processString(node.stringValue, root, i);

			case '{':

				node.valueDataType = JSON_DATATYPE_OBJECT;
				return processObject(node.objectValue, root, i);

			case '[':
				
				node.valueDataType = JSON_DATATYPE_ARRAY;
				return processArray(node.arrayValue, root, i);

			case 't':
			case 'f':

				node.valueDataType = JSON_DATATYPE_BOOLEAN;
				return processBoolean(node.booleanValue, root, i);
			
			case 'n':

				node.valueDataType = JSON_DATATYPE_NULL;
				return processNull(root, i);
				
			default:

				node.valueDataType = JSON_DATATYPE_NUMERIC;
				return processNumeric(node.numericValue, root, i);
		}
	}
}

//-------------------------------------------------------------------

unsigned long 
json::processBoolean(bool &node, 
					const dodoString &root, 
					unsigned long pos)
{
	if ((root.size() - pos) < 4)
		throw baseEx(ERRMODULE_JSON, JSONEX_PROCESSBOOLEAN, ERR_LIBDODO, JSONEX_MALFORMEDJSON, JSONEX_MALFORMEDJSON_STR, __LINE__, __FILE__);
	
	if (root.substr(pos, 4) == "true")
	{
		node = true;
		
		return pos + 3;
	}
	else
	{
		if (root.substr(pos, 5) == "false")
		{
			node = false;
			
			return pos + 4;
		}
		else
			throw baseEx(ERRMODULE_JSON, JSONEX_PROCESSBOOLEAN, ERR_LIBDODO, JSONEX_MALFORMEDJSON, JSONEX_MALFORMEDJSON_STR, __LINE__, __FILE__);
	}

	return pos;
}

//-------------------------------------------------------------------

unsigned long 
json::processNull(const dodoString &root, 
				unsigned long pos)
{
	if ((root.size() - pos) < 4)
		throw baseEx(ERRMODULE_JSON, JSONEX_PROCESSNULL, ERR_LIBDODO, JSONEX_MALFORMEDJSON, JSONEX_MALFORMEDJSON_STR, __LINE__, __FILE__);
	
	if (root.substr(pos, 4) == "null")	
		return pos + 3;
	else
		throw baseEx(ERRMODULE_JSON, JSONEX_PROCESSNULL, ERR_LIBDODO, JSONEX_MALFORMEDJSON, JSONEX_MALFORMEDJSON_STR, __LINE__, __FILE__);

	return pos;
	
}

//-------------------------------------------------------------------

unsigned long 
json::processNumeric(long &node, 
					const dodoString &root, 
					unsigned long pos)
{
	dodoString numeric;
	
	unsigned long i(pos), j(root.size());
	for (;i<j;++i)
	{
		switch (root[i])
		{
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '0':
			case '+':
			case '-':
			case 'e':
			case 'E': 
				
				numeric.append(1, root[i]);
				
				break;
				
			default:
				
				node = stringTools::stringToL(numeric);
				
				return i;
		}
	}
	
	node = stringTools::stringToL(numeric);
	
	return i;
}

//-------------------------------------------------------------------

unsigned long
json::processObject(dodoMap<dodoString, jsonNode, stringTools::equal> &node, 
					const dodoString &root, 
					unsigned long pos)
{
	node.clear();
	
	short state = JSON_STATE_OBJECT_INITIAL;
	
	jsonNode subNodeValue;
	dodoString subNodeName;
	
	unsigned long i(pos), j(root.size());
	for (;i<j;++i)
	{
		int o = 0;
		switch (root[i])
		{
			case ' ':
			case '\r':
			case '\n':
			case '\t':
				
				break;
			
			case '{':
				
				if (state == JSON_STATE_OBJECT_INITIAL)
					state = JSON_STATE_OBJECT_OBJECTNAME;
				else
				{
					if (state == JSON_STATE_OBJECT_OBJECTVALUE)
					{
						i = processValue(subNodeValue, root, i);
						node.insert(subNodeName, subNodeValue);
						
						state = JSON_STATE_OBJECT_OBJECTNAME;
					}					
				}
				
				break;
				
			case '}':
				
				return i;
				
			case ':':
					
				break;
				
			case ',':
				
				break;

			case '"':
				
				if (state == JSON_STATE_OBJECT_OBJECTNAME)
				{
					i = processString(subNodeName, root, i);
					
					state = JSON_STATE_OBJECT_OBJECTVALUE;
					
					break;
				}
				else
				{
					if (state == JSON_STATE_OBJECT_OBJECTVALUE)
					{
						i = processValue(subNodeValue, root, i);
						node.insert(subNodeName, subNodeValue);
						
						state = JSON_STATE_OBJECT_OBJECTNAME;
					}
				}
				
			default:

				if (state == JSON_STATE_OBJECT_OBJECTVALUE)
				{
					i = processValue(subNodeValue, root, i);
					node.insert(subNodeName, subNodeValue);
					
					state = JSON_STATE_OBJECT_OBJECTNAME;
				}
		}
	}
	
	return i;
}

//-------------------------------------------------------------------

void 
json::processJSON(jsonNode &node,
				const dodoString &root)
{
	node.clear();
	
	node.valueDataType = JSON_DATATYPE_OBJECT;
	processObject(node.objectValue, root, 0);
}

//-------------------------------------------------------------------

unsigned long 
json::processString(dodoString &node, 
					const dodoString &root, 
					unsigned long pos)
{	
	node.clear();
	
	bool escape = false;
	short state = JSON_STATE_STRING_INITIAL;
	
	unsigned long i(pos), j(root.size());
	for (;i<j;++i)
	{
		switch (root[i])
		{
			case '\\':
				
				if (!escape)
				{
					escape = true;

					break;
				}
				
			case '"':
				
				if (!escape)
				{
					if (state == JSON_STATE_STRING_INITIAL)
					{
						state = JSON_STATE_STRING_STRING;
						
						break;
					}
					else
						return i;
				}
			
			default:
				
				if (escape)
				{
					escape = false;

					node.append(1, '\\');
					node.append(1, root[i]);
				}
				else
					node.append(1, root[i]);
		}
	}
	
	return i;
}

//-------------------------------------------------------------------
