/***************************************************************************
 *            dataFormatJsonProcessor.cc
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/dataFormatJsonProcessor.h>

using namespace dodo::data::format::json;

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
		case DATATYPE_STRING:
		{

			dodoString jsonObject = "\"";
			dodoString stringValue = root.stringValue;
			tools::string::replace("\"", "\\\"", stringValue);
			jsonObject.append(stringValue);
			jsonObject.append("\"");

			return jsonObject;
		}

		case DATATYPE_OBJECT:
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

		case DATATYPE_ARRAY:
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

		case DATATYPE_NUMERIC:

			return tools::string::lToString(root.numericValue);

		case DATATYPE_BOOLEAN:

			return root.booleanValue ? "true" : "false";

		case DATATYPE_NULL:
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
			case ' ':
			case '\r':
			case '\n':
			case '\t':

				break;

			case '"':

				node.valueDataType = DATATYPE_STRING;

				return processString(node.stringValue, root, i);

			case '{':

				node.valueDataType = DATATYPE_OBJECT;

				return processObject(node.objectValue, root, i);

			case '[':

				node.valueDataType = DATATYPE_ARRAY;

				return processArray(node.arrayValue, root, i);

			case 't':
			case 'f':

				node.valueDataType = DATATYPE_BOOLEAN;

				return processBoolean(node.booleanValue, root, i);

			case 'n':

				node.valueDataType = DATATYPE_NULL;

				return processNull(root, i);

			default:

				node.valueDataType = DATATYPE_NUMERIC;

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
		throw exception::basic(exception::ERRMODULE_DATAFORMATJSONPROCESSOR, PROCESSOREX_PROCESSBOOLEAN, exception::ERRNO_LIBDODO, PROCESSOREX_MALFORMEDJSONBOOLEAN, DATAFORMATJSONPROCESSOREX_MALFORMEDJSONBOOLEAN_STR, __LINE__, __FILE__);

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
			throw exception::basic(exception::ERRMODULE_DATAFORMATJSONPROCESSOR, PROCESSOREX_PROCESSBOOLEAN, exception::ERRNO_LIBDODO, PROCESSOREX_MALFORMEDJSONBOOLEAN, DATAFORMATJSONPROCESSOREX_MALFORMEDJSONBOOLEAN_STR, __LINE__, __FILE__);
	}

	return pos;
}

//-------------------------------------------------------------------

unsigned long
processor::processNull(const dodoString &root,
					   unsigned long pos)
{
	if ((root.size() - pos) < 4)
		throw exception::basic(exception::ERRMODULE_DATAFORMATJSONPROCESSOR, PROCESSOREX_PROCESSNULL, exception::ERRNO_LIBDODO, PROCESSOREX_MALFORMEDJSONNULL, DATAFORMATJSONPROCESSOREX_MALFORMEDJSONNULL_STR, __LINE__, __FILE__);

	if (root.substr(pos, 4) == "null")
		return pos + 3;
	else
		throw exception::basic(exception::ERRMODULE_DATAFORMATJSONPROCESSOR, PROCESSOREX_PROCESSNULL, exception::ERRNO_LIBDODO, PROCESSOREX_MALFORMEDJSONNULL, DATAFORMATJSONPROCESSOREX_MALFORMEDJSONNULL_STR, __LINE__, __FILE__);

	return pos;

}

//-------------------------------------------------------------------

unsigned long
processor::processNumeric(long &node,
						  const dodoString &root,
						  unsigned long pos)
{
	dodoString numeric;

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

			case ',':
			case ']':
			case '}':

				node = tools::string::stringToL(numeric);

				return i - 1;

			default:

				throw exception::basic(exception::ERRMODULE_DATAFORMATJSONPROCESSOR, PROCESSOREX_PROCESSNUMERIC, exception::ERRNO_LIBDODO, PROCESSOREX_MALFORMEDJSONNUMERIC, DATAFORMATJSONPROCESSOREX_MALFORMEDJSONNUMERIC_STR, __LINE__, __FILE__);
		}
	}


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
		switch (root[i])
		{
			case ' ':
			case '\r':
			case '\n':
			case '\t':
			case ':':
			case ',':

				break;

			case '{':

				if (state == JSON_STATE_OBJECT_INITIAL)
				{
					state = JSON_STATE_OBJECT_OBJECTNAME;

					break;
				}
				else
				{
					if (state == JSON_STATE_OBJECT_OBJECTVALUE)
					{
						subNodeValue.clear();

						subNodeValue.valueDataType = DATATYPE_OBJECT;

						i = processObject(subNodeValue.objectValue, root, i);
						jnode.insert(make_pair(subNodeName, subNodeValue));

						state = JSON_STATE_OBJECT_OBJECTNAME;

						break;
					}
				}

			case '}':

				return i;

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
						subNodeValue.clear();

						subNodeValue.valueDataType = DATATYPE_STRING;

						i = processString(subNodeValue.stringValue, root, i);
						jnode.insert(make_pair(subNodeName, subNodeValue));

						state = JSON_STATE_OBJECT_OBJECTNAME;

						break;
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
processor::processString(const dodoString &root)
{
	node node;

	node.valueDataType = DATATYPE_OBJECT;
	processObject(node.objectValue, root, 0);

	return node;
}

//-------------------------------------------------------------------

node
processor::processFile(const dodoString &path)
{
	node node;

	node.valueDataType = DATATYPE_OBJECT;
	processObject(node.objectValue, tools::filesystem::getFileContents(path), 0);

	return node;
}

//-------------------------------------------------------------------

dodoString
processor::fromMap(const dodoStringMap &root)
{
	node nodeDef;
	node subNodeDef;

	nodeDef.valueDataType = DATATYPE_OBJECT;
	subNodeDef.valueDataType = DATATYPE_STRING;

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
	node JSON = processString(jnode);

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

