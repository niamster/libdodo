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

json::json()
{
}

//-------------------------------------------------------------------

json::~json()
{
}

//-------------------------------------------------------------------

dodoString
json::makeJSON(const jsonNode &root)
{
	switch (root.valueDataType)
	{
		case JSON_DATATYPE_STRING:
		{

			dodoString jsonObject = "\"";
			dodoString stringValue = root.stringValue;
			toolsString::replace("\"", "\\\"", stringValue);
			jsonObject.append(stringValue);
			jsonObject.append("\"");

			return jsonObject;
		}

		case JSON_DATATYPE_OBJECT:
		{
			dodoString jsonObject = "{";

			dodoMap<dodoString, jsonNode, dodoMapStringCompare>::const_iterator i = root.objectValue.begin(), j = root.objectValue.end();
			if (i != j)
			{
				for (--j; i != j; ++i)
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

			dodoArray<jsonNode>::const_iterator i = root.arrayValue.begin(), j = root.arrayValue.end();
			if (i != j)
			{
				--j;
				for (; i != j; ++i)
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

			return toolsString::lToString(root.numericValue);

		case JSON_DATATYPE_BOOLEAN:

			return root.booleanValue ? "true" : "false";

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

	bool initial = true;

	jsonNode subNode;

	unsigned long i(pos), j(root.size());
	for (; i < j; ++i)
	{
		switch (root[i])
		{
			case ' ':
			case '\r':
			case '\n':
			case '\t':

				break;

			case '[':

				if (initial)
					initial = false;
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
	for (; i < j; ++i)
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
		throw baseEx(ERRMODULE_JSON, JSONEX_PROCESSBOOLEAN, ERR_LIBDODO, JSONEX_MALFORMEDJSONBOOLEAN, JSONEX_MALFORMEDJSONBOOLEAN_STR, __LINE__, __FILE__);

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
			throw baseEx(ERRMODULE_JSON, JSONEX_PROCESSBOOLEAN, ERR_LIBDODO, JSONEX_MALFORMEDJSONBOOLEAN, JSONEX_MALFORMEDJSONBOOLEAN_STR, __LINE__, __FILE__);
	}

	return pos;
}

//-------------------------------------------------------------------

unsigned long
json::processNull(const dodoString &root,
				  unsigned long pos)
{
	if ((root.size() - pos) < 4)
		throw baseEx(ERRMODULE_JSON, JSONEX_PROCESSNULL, ERR_LIBDODO, JSONEX_MALFORMEDJSONNULL, JSONEX_MALFORMEDJSONNULL_STR, __LINE__, __FILE__);

	if (root.substr(pos, 4) == "null")
		return pos + 3;
	else
		throw baseEx(ERRMODULE_JSON, JSONEX_PROCESSNULL, ERR_LIBDODO, JSONEX_MALFORMEDJSONNULL, JSONEX_MALFORMEDJSONNULL_STR, __LINE__, __FILE__);

	return pos;

}

//-------------------------------------------------------------------

unsigned long
json::processNumeric(long &node,
					 const dodoString &root,
					 unsigned long pos)
{
	dodoString numeric;

	bool endOfNumeric = false;

	unsigned long i(pos), j(root.size());
	for (; i < j; ++i)
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
			case ' ':

				numeric.append(1, root[i]);

				break;

			case ',':

				endOfNumeric = true;

				break;

			default:

				throw baseEx(ERRMODULE_JSON, JSONEX_PROCESSNUMERIC, ERR_LIBDODO, JSONEX_MALFORMEDJSONNUMERIC, numeric, __LINE__, __FILE__);
		}

		if (endOfNumeric)
			break;
	}

	node = toolsString::stringToL(numeric);

	return i;
}

//-------------------------------------------------------------------

unsigned long
json::processObject(dodoMap<dodoString, jsonNode, dodoMapStringCompare> &node,
					const dodoString &root,
					unsigned long pos)
{
	node.clear();

	short state = JSON_STATE_OBJECT_INITIAL;

	jsonNode subNodeValue;
	dodoString subNodeName;

	unsigned long i(pos), j(root.size());
	for (; i < j; ++i)
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
						node.insert(make_pair(subNodeName, subNodeValue));

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
						node.insert(make_pair(subNodeName, subNodeValue));

						state = JSON_STATE_OBJECT_OBJECTNAME;
					}
				}

			default:

				if (state == JSON_STATE_OBJECT_OBJECTVALUE)
				{
					i = processValue(subNodeValue, root, i);
					node.insert(make_pair(subNodeName, subNodeValue));

					state = JSON_STATE_OBJECT_OBJECTNAME;
				}
		}
	}

	return i;
}

//-------------------------------------------------------------------

jsonNode
json::processJSON(const dodoString &root)
{
	jsonNode node;

	node.valueDataType = JSON_DATATYPE_OBJECT;
	processObject(node.objectValue, root, 0);

	return node;
}

//-------------------------------------------------------------------

dodoString
json::mapToJSON(const dodoStringMap &root)
{
	jsonNode nodeDef;
	jsonNode subNodeDef;

	nodeDef.valueDataType = JSON_DATATYPE_OBJECT;
	subNodeDef.valueDataType = JSON_DATATYPE_STRING;

	dodoStringMap::const_iterator i = root.begin(), j = root.end();
	for (; i != j; ++i)
	{
		subNodeDef.stringValue = i->second;
		nodeDef.objectValue.insert(make_pair(i->first, subNodeDef));
	}

	return makeJSON(nodeDef);
}

//-------------------------------------------------------------------

dodoStringMap
json::JSONToMap(const dodoString &node)
{
	jsonNode JSON = processJSON(node);

	dodoStringMap map;

	dodoMap<dodoString, jsonNode, dodoMapStringCompare>::iterator i = JSON.objectValue.begin(), j = JSON.objectValue.end();
	for (; i != j; ++i)
		map.insert(make_pair(i->first, i->second.stringValue));

	return map;
}

//-------------------------------------------------------------------

unsigned long
json::processString(dodoString &node,
					const dodoString &root,
					unsigned long pos)
{
	node.clear();

	bool escape = false;
	bool initial = true;

	unsigned long i(pos), j(root.size());
	for (; i < j; ++i)
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
					if (initial)
					{
						initial = false;

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

