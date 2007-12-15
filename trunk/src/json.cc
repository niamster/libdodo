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
	
				jsonObject.append("}");
				
				return jsonObject;
			}
			
		case JSON_DATATYPE_ARRAY:
			{
				dodoString jsonObject = "[";
				
				dodoArray<jsonNodeDef>::const_iterator i = root.arrayValue.begin(), j = root.arrayValue.end() - 1;
				for (;i!=j;++i)
				{
					jsonObject.append(makeJSON(*i));
					jsonObject.append(",");
				}
				jsonObject.append(makeJSON(*i));
				
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

void 
json::processJSON(jsonNode &node,
				const dodoString &root)
{
	unsigned long i(0), j(root.size());
	for (;i<j;++i)
	{
		switch (root[i])
		{
			case '"':
				
				i = processString(node, root, i + 1);
				
				continue;
			
			case ' ':
			case '\r':
			case '\n':
			case '\t':
				
				continue;
			
			break;
			
			case '{':
				
				
				break;
				
			case '}':
				
				break;
				
			case ':':
				
				break;
				
			case ',':
				
				break;
				
			case '[':
				
				break;
				
			case ']':
				
				break;
		}
	}
}

//-------------------------------------------------------------------

unsigned long 
json::processString(jsonNode &node, 
					const dodoString &root, 
					unsigned long pos)
{
	
	node.valueDataType = JSON_DATATYPE_STRING;
	
	bool escape = false;
	
	unsigned long i(pos), j(root.size());
	for (;i<j;++i)
	{
		switch (root[i])
		{
			case '\\':
				
				if (escape)
				{
					escape = false;
					
					node.stringValue.append("\\");
				}
				escape = true;
			
				break;
				
			case '"':
				
				if (escape)
				{
					escape = false;
					
					node.stringValue.append("\\\"");
				}
				else
					return (i + 1);
				
				break;
				
			case 'r':
			case 'n':
			case 't':
			case 'b':
			case 'f':
			case '/':
				
				if (escape)
				{
					escape = false;
					
					processEscaped(node, root[i]);
				}
				else
					node.stringValue.append(1, root[i]);
			
			default:
				
				node.stringValue.append(1, root[i]);
			
				break;
		}
	}	
	
	return i;
}

//-------------------------------------------------------------------

void 
json::processEscaped(jsonNode &node, 
					char symbol)
{
	switch (symbol)
	{
		case 'n':
			
			node.stringValue.append("\n");
			
			break;
			
		case 'r':
			
			node.stringValue.append("\r");
			
			break;
			
		case 't':
			
			node.stringValue.append("\t");
			
			break;
			
		case '/':
			
			node.stringValue.append("\\/");
			
			break;
			
		case 'b':
			
			node.stringValue.append("\b");
			
			break;
			
		case 'f':
			
			node.stringValue.append("\f");
			
			break;
	}
}

//-------------------------------------------------------------------
