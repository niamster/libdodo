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

#include <libdodo/directives.h>

#include <libdodo/dataFormatJsonProcessor.h>
#include <libdodo/types.h>
#include <libdodo/toolsString.h>
#include <libdodo/toolsFilesystem.h>
#include <libdodo/dataFormatJsonProcessorEx.h>
#include <libdodo/dataFormatJsonNode.h>
#include <libdodo/ioChannel.h>

using namespace dodo::data::format::json;

processor::processor()
{
}

//-------------------------------------------------------------------

processor::~processor()
{
}

//-------------------------------------------------------------------

void
processor::make(const node        &root,
				const io::channel &io)
{
	switch (root.valueDataType) {
		case DATATYPE_STRING:
		{
			io.writeStream("\"");
			dodoString stringValue = *root.stringValue;
			tools::string::replace("\"", "\\\"", stringValue);
			io.writeStream(stringValue);
			io.writeStream("\"");

			break;
		}

		case DATATYPE_OBJECT:
		{
			io.writeStream("{");

			dodoMap<dodoString, node, dodoMapStringCompare>::const_iterator
			i = root.objectValue->begin(),
			j = root.objectValue->end();
			if (i != j) {
				for (--j; i != j; ++i) {
					io.writeStream("\"");
					io.writeStream(i->first);
					io.writeStream("\":");

					make(i->second, io);
					io.writeStream(",");
				}
				io.writeStream("\"");
				io.writeStream(i->first);
				io.writeStream("\":");

				make(i->second, io);
			}

			io.writeStream("}");

			break;
		}

		case DATATYPE_ARRAY:
		{
			io.writeStream("[");

			dodoArray<node>::const_iterator
			i = root.arrayValue->begin(),
			j = root.arrayValue->end();
			if (i != j) {
				--j;
				for (; i != j; ++i) {
					make(*i, io);
					io.writeStream(",");
				}
				make(*i, io);
			}

			io.writeStream("]");

			break;
		}

		case DATATYPE_NUMERIC:
			io.writeStream(tools::string::lToString(root.numericValue));

			break;

		case DATATYPE_BOOLEAN:
			io.writeStream(root.booleanValue ? "true" : "false");

			break;

		case DATATYPE_NULL:
		default:
			io.writeStream("null");
	}
}

//-------------------------------------------------------------------

unsigned long
processor::processArray(dodoArray<node>  &jnode,
						const dodoString &root,
						unsigned long    pos)
{
	bool initial = true;

	node subNode;

	unsigned long i(pos), j(root.size());
	for (; i < j; ++i) {
		switch (root[i]) {
			case ' ':
			case '\r':
			case '\n':
			case '\t':
				break;

			case '[':
				if (initial)
					initial = false;
				else {
					i = processValue(subNode, root, i);

					jnode.push_back(subNode);

					subNode.~node();
					subNode.valueDataType = DATATYPE_NULL;
				}

				break;

			case ']':
				return i;

			case ',':
				break;

			default:
				i = processValue(subNode, root, i);

				jnode.push_back(subNode);

				subNode.~node();
				subNode.valueDataType = DATATYPE_NULL;
		}
	}

	return i;
}

//-------------------------------------------------------------------

unsigned long
processor::processValue(node             &node,
						const dodoString &root,
						unsigned long    pos)
{
	unsigned long i(pos), j(root.size());
	for (; i < j; ++i) {
		switch (root[i]) {
			case ' ':
			case '\r':
			case '\n':
			case '\t':
				break;

			case '"':
				node.valueDataType = DATATYPE_STRING;
				node.stringValue = new dodoString;

				return processString(*node.stringValue, root, i);

			case '{':
				node.valueDataType = DATATYPE_OBJECT;
				node.objectValue = new dodoMap<dodoString, json::node, dodoMapStringCompare>;

				return processObject(*node.objectValue, root, i);

			case '[':
				node.valueDataType = DATATYPE_ARRAY;
				node.arrayValue = new dodoArray<json::node>;

				return processArray(*node.arrayValue, root, i);

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

	return (unsigned long)-1;
}

//-------------------------------------------------------------------

unsigned long
processor::processBoolean(bool             &node,
						  const dodoString &root,
						  unsigned long    pos)
{
	if ((root.size() - pos) < 4)
		throw exception::basic(exception::ERRMODULE_DATAFORMATJSONPROCESSOR, PROCESSOREX_PROCESSBOOLEAN, exception::ERRNO_LIBDODO, PROCESSOREX_MALFORMEDJSONBOOLEAN, DATAFORMATJSONPROCESSOREX_MALFORMEDJSONBOOLEAN_STR, __LINE__, __FILE__);

	if (root[pos] == 't' && root[pos + 1] == 'r' && root[pos + 2] == 'u' && root[pos + 3] == 'e') {
		node = true;

		return pos + 3;
	} else {
		if (root[pos] == 'f' && root[pos + 1] == 'a' && root[pos + 2] == 'l' && root[pos + 3] == 's' && root[pos + 4] == 'e') {
			node = false;

			return pos + 4;
		} else
			throw exception::basic(exception::ERRMODULE_DATAFORMATJSONPROCESSOR, PROCESSOREX_PROCESSBOOLEAN, exception::ERRNO_LIBDODO, PROCESSOREX_MALFORMEDJSONBOOLEAN, DATAFORMATJSONPROCESSOREX_MALFORMEDJSONBOOLEAN_STR, __LINE__, __FILE__);
	}

	return pos;
}

//-------------------------------------------------------------------

unsigned long
processor::processNull(const dodoString &root,
					   unsigned long    pos)
{
	if ((root.size() - pos) < 4)
		throw exception::basic(exception::ERRMODULE_DATAFORMATJSONPROCESSOR, PROCESSOREX_PROCESSNULL, exception::ERRNO_LIBDODO, PROCESSOREX_MALFORMEDJSONNULL, DATAFORMATJSONPROCESSOREX_MALFORMEDJSONNULL_STR, __LINE__, __FILE__);

	if (root[pos] == 'n' && root[pos + 1] == 'u' && root[pos + 2] == 'l' && root[pos + 3] == 'l')
		return pos + 3;
	else
		throw exception::basic(exception::ERRMODULE_DATAFORMATJSONPROCESSOR, PROCESSOREX_PROCESSNULL, exception::ERRNO_LIBDODO, PROCESSOREX_MALFORMEDJSONNULL, DATAFORMATJSONPROCESSOREX_MALFORMEDJSONNULL_STR, __LINE__, __FILE__);

	return pos;
}

//-------------------------------------------------------------------

unsigned long
processor::processNumeric(long             &node,
						  const dodoString &root,
						  unsigned long    pos)
{
	dodoString numeric;

	unsigned long i(pos), j(root.size());
	for (; i < j; ++i) {
		switch (root[i]) {
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
	short state = JSON_STATE_OBJECT_INITIAL;

	node subNodeValue;
	dodoString subNodeName;

	unsigned long i(pos), j(root.size());
	for (; i < j; ++i) {
		switch (root[i]) {
			case ' ':
			case '\r':
			case '\n':
			case '\t':
			case ':':
			case ',':
				break;

			case '{':
				if (state == JSON_STATE_OBJECT_INITIAL)
					state = JSON_STATE_OBJECT_OBJECTNAME;
				else {
					if (state == JSON_STATE_OBJECT_OBJECTVALUE) {
						subNodeValue.valueDataType = DATATYPE_OBJECT;
						subNodeValue.objectValue = new dodoMap<dodoString, node, dodoMapStringCompare>;

						i = processObject(*subNodeValue.objectValue, root, i);
						jnode.insert(make_pair(subNodeName, subNodeValue));

						delete subNodeValue.objectValue;
						subNodeValue.valueDataType = DATATYPE_NULL;

						state = JSON_STATE_OBJECT_OBJECTNAME;
					}
				}

				break;

			case '}':
				return i;

			case '"':
				if (state == JSON_STATE_OBJECT_OBJECTNAME) {
					subNodeName.clear();
					i = processString(subNodeName, root, i);

					state = JSON_STATE_OBJECT_OBJECTVALUE;
				} else {
					if (state == JSON_STATE_OBJECT_OBJECTVALUE) {
						subNodeValue.valueDataType = DATATYPE_STRING;
						subNodeValue.stringValue = new dodoString;

						i = processString(*subNodeValue.stringValue, root, i);
						jnode.insert(make_pair(subNodeName, subNodeValue));

						delete subNodeValue.stringValue;
						subNodeValue.valueDataType = DATATYPE_NULL;

						state = JSON_STATE_OBJECT_OBJECTNAME;
					}
				}

				break;

			default:
				if (state == JSON_STATE_OBJECT_OBJECTVALUE) {
					i = processValue(subNodeValue, root, i);
					jnode.insert(make_pair(subNodeName, subNodeValue));

					subNodeValue.~node();
					subNodeValue.valueDataType = DATATYPE_NULL;

					state = JSON_STATE_OBJECT_OBJECTNAME;
				}
		}
	}

	return i;
}

//-------------------------------------------------------------------

node
processor::process(const io::channel &io)
{
	node node;

	node.valueDataType = DATATYPE_OBJECT;
	node.objectValue = new dodoMap<dodoString, json::node, dodoMapStringCompare>;

	dodoString json, jsonPart;

	while (true) {
		jsonPart = io.readStream();
		if (jsonPart.size() == 0)
			break;

		json.append(jsonPart);
	}
	jsonPart.clear();

	processObject(*node.objectValue, json, 0);

	return node;
}

//-------------------------------------------------------------------

void
processor::fromMap(const dodoStringMap &root,
				   const io::channel   &io)
{
	node nodeDef;
	node subNodeDef;

	nodeDef.valueDataType = DATATYPE_OBJECT;
	nodeDef.objectValue = new dodoMap<dodoString, json::node, dodoMapStringCompare>;

	subNodeDef.valueDataType = DATATYPE_STRING;

	dodoStringMap::const_iterator i = root.begin(), j = root.end();
	for (; i != j; ++i) {
		subNodeDef.stringValue = new dodoString(i->second);

		nodeDef.objectValue->insert(make_pair(i->first, subNodeDef));
		delete subNodeDef.stringValue;
	}
	subNodeDef.valueDataType = DATATYPE_NULL;

	make(nodeDef, io);
}

//-------------------------------------------------------------------

dodo::dodoStringMap
processor::toMap(const io::channel &io)
{
	node JSON = process(io);

	dodoStringMap map;

	dodoMap<dodoString, node, dodoMapStringCompare>::iterator
	i = JSON.objectValue->begin(),
	j = JSON.objectValue->end();
	for (; i != j; ++i)
		if (i->second.valueDataType == DATATYPE_STRING)
			map.insert(make_pair(i->first, *i->second.stringValue));

	return map;
}

//-------------------------------------------------------------------

unsigned long
processor::processString(dodoString       &jnode,
						 const dodoString &root,
						 unsigned long    pos)
{
	bool escape = false;
	bool initial = true;

	unsigned long i(pos), j(root.size());
	for (; i < j; ++i) {
		switch (root[i]) {
			case '\\':
				if (!escape) {
					escape = true;

					break;
				}

			case '"':
				if (!escape) {
					if (initial) {
						initial = false;

						break;
					} else
						return i;
				}

			default:
				if (escape) {
					escape = false;

					jnode.append(1, '\\');
					jnode.append(1, root[i]);
				} else
					jnode.append(1, root[i]);
		}
	}

	return i;
}

//-------------------------------------------------------------------

