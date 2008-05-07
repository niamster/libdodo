/***************************************************************************
 *            jsonProcessor.cc
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

#include <libdodo/jsonProcessor.h>

using namespace dodo::json;

processor::processor()
{
}

//-------------------------------------------------------------------

processor::~processor()
{
}

//-------------------------------------------------------------------

dodoString
processor::make(const node &root)
{
	switch (root.valueDataType)
	{
		case NODE_DATATYPE_STRING:
		{

			dodoString jsonObject = "\"";
			dodoString stringValue = root.stringValue;
			toolsString::replace("\"", "\\\"", stringValue);
			jsonObject.append(stringValue);
			jsonObject.append("\"");

			return jsonObject;
		}

		case NODE_DATATYPE_OBJECT:
		{
			dodoString jsonObject = "{";

			dodoMap<dodoString, node, dodoMapStringCompare>::const_iterator i = root.objectValue.begin(), j = root.objectValue.end();
			if (i != j)
			{
				for (--j; i != j; ++i)
				{
					jsonObject.append("\"");
					jsonObject.append(i->first);
					jsonObject.append("\":");

					jsonObject.append(make(i->second));
					jsonObject.append(",");
				}
				jsonObject.append("\"");
				jsonObject.append(i->first);
				jsonObject.append("\":");

				jsonObject.append(make(i->second));
			}

			jsonObject.append("}");

			return jsonObject;
		}

		case NODE_DATATYPE_ARRAY:
		{
			dodoString jsonObject = "[";

			dodoArray<node>::const_iterator i = root.arrayValue.begin(), j = root.arrayValue.end();
			if (i != j)
			{
				--j;
				for (; i != j; ++i)
				{
					jsonObject.append(make(*i));
					jsonObject.append(",");
				}
				jsonObject.append(make(*i));
			}

			jsonObject.append("]");

			return jsonObject;
		}

		case NODE_DATATYPE_NUMERIC:

			return toolsString::lToString(root.numericValue);

		case NODE_DATATYPE_BOOLEAN:

			return root.booleanValue ? "true" : "false";

		case NODE_DATATYPE_NULL:
		default:

			return "null";
	}

	return "null";
}

//-------------------------------------------------------------------

unsigned long
processor::processArray(dodoArray<node> &jnode,
				   const dodoString &root,
				   unsigned long pos)
{
	jnode.clear();

	bool initial = true;

	node subNode;

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

					jnode.push_back(subNode);
				}

				break;

			case ']':

				return i;

			case ',':

				break;

			default:

				i = processValue(subNode, root, i);

				jnode.push_back(subNode);
		}
	}

	return i;
}

//-------------------------------------------------------------------

unsigned long
processor::processValue(node &node,
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

				node.valueDataType = NODE_DATATYPE_STRING;
				return processString(node.stringValue, root, i);

			case '{':

				node.valueDataType = NODE_DATATYPE_OBJECT;
				return processObject(node.objectValue, root, i);

			case '[':

				node.valueDataType = NODE_DATATYPE_ARRAY;
				return processArray(node.arrayValue, root, i);

			case 't':
			case 'f':

				node.valueDataType = NODE_DATATYPE_BOOLEAN;
				return processBoolean(node.booleanValue, root, i);

			case 'n':

				node.valueDataType = NODE_DATATYPE_NULL;
				return processNull(root, i);

			default:

				node.valueDataType = NODE_DATATYPE_NUMERIC;
				return processNumeric(node.numericValue, root, i);
		}
	}
}

//-------------------------------------------------------------------

unsigned long
processor::processBoolean(bool &node,
					 const dodoString &root,
					 unsigned long pos)
{
	if ((root.size() - pos) < 4)
		throw baseEx(ERRMODULE_JSONPROCESSOR, PROCESSOREX_PROCESSBOOLEAN, ERR_LIBDODO, PROCESSOREX_MALFORMEDJSONBOOLEAN, PROCESSOREX_MALFORMEDJSONBOOLEAN_STR, __LINE__, __FILE__);

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
			throw baseEx(ERRMODULE_JSONPROCESSOR, PROCESSOREX_PROCESSBOOLEAN, ERR_LIBDODO, PROCESSOREX_MALFORMEDJSONBOOLEAN, PROCESSOREX_MALFORMEDJSONBOOLEAN_STR, __LINE__, __FILE__);
	}

	return pos;
}

//-------------------------------------------------------------------

unsigned long
processor::processNull(const dodoString &root,
				  unsigned long pos)
{
	if ((root.size() - pos) < 4)
		throw baseEx(ERRMODULE_JSONPROCESSOR, PROCESSOREX_PROCESSNULL, ERR_LIBDODO, PROCESSOREX_MALFORMEDJSONNULL, PROCESSOREX_MALFORMEDJSONNULL_STR, __LINE__, __FILE__);

	if (root.substr(pos, 4) == "null")
		return pos + 3;
	else
		throw baseEx(ERRMODULE_JSONPROCESSOR, PROCESSOREX_PROCESSNULL, ERR_LIBDODO, PROCESSOREX_MALFORMEDJSONNULL, PROCESSOREX_MALFORMEDJSONNULL_STR, __LINE__, __FILE__);

	return pos;

}

//-------------------------------------------------------------------

unsigned long
processor::processNumeric(long &node,
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

				throw baseEx(ERRMODULE_JSONPROCESSOR, PROCESSOREX_PROCESSNUMERIC, ERR_LIBDODO, PROCESSOREX_MALFORMEDJSONNUMERIC, numeric, __LINE__, __FILE__);
		}

		if (endOfNumeric)
			break;
	}

	node = toolsString::stringToL(numeric);

	return i;
}

//-------------------------------------------------------------------

unsigned long
processor::processObject(dodoMap<dodoString, node, dodoMapStringCompare> &jnode,
					const dodoString &root,
					unsigned long pos)
{
	jnode.clear();

	short state = JSON_STATE_OBJECT_INITIAL;

	node subNodeValue;
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
						jnode.insert(make_pair(subNodeName, subNodeValue));

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
						jnode.insert(make_pair(subNodeName, subNodeValue));

						state = JSON_STATE_OBJECT_OBJECTNAME;
					}
				}

			default:

				if (state == JSON_STATE_OBJECT_OBJECTVALUE)
				{
					i = processValue(subNodeValue, root, i);
					jnode.insert(make_pair(subNodeName, subNodeValue));

					state = JSON_STATE_OBJECT_OBJECTNAME;
				}
		}
	}

	return i;
}

//-------------------------------------------------------------------

node
processor::process(const dodoString &root)
{
	node node;

	node.valueDataType = NODE_DATATYPE_OBJECT;
	processObject(node.objectValue, root, 0);

	return node;
}

//-------------------------------------------------------------------

dodoString
processor::fromMap(const dodoStringMap &root)
{
	node nodeDef;
	node subNodeDef;

	nodeDef.valueDataType = NODE_DATATYPE_OBJECT;
	subNodeDef.valueDataType = NODE_DATATYPE_STRING;

	dodoStringMap::const_iterator i = root.begin(), j = root.end();
	for (; i != j; ++i)
	{
		subNodeDef.stringValue = i->second;
		nodeDef.objectValue.insert(make_pair(i->first, subNodeDef));
	}

	return make(nodeDef);
}

//-------------------------------------------------------------------

dodo::dodoStringMap
processor::toMap(const dodoString &jnode)
{
	node JSON = process(jnode);

	dodoStringMap map;

	dodoMap<dodoString, node, dodoMapStringCompare>::iterator i = JSON.objectValue.begin(), j = JSON.objectValue.end();
	for (; i != j; ++i)
		map.insert(make_pair(i->first, i->second.stringValue));

	return map;
}

//-------------------------------------------------------------------

unsigned long
processor::processString(dodoString &jnode,
					const dodoString &root,
					unsigned long pos)
{
	jnode.clear();

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

					jnode.append(1, '\\');
					jnode.append(1, root[i]);
				}
				else
					jnode.append(1, root[i]);
		}
	}

	return i;
}

//-------------------------------------------------------------------

