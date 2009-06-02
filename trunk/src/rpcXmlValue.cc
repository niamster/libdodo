/***************************************************************************
 *            rpcXmlValue.cc
 *
 *  Wed Apr 09 23:30:55 2008
 *  Copyright  2008  Ni@m
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

#include <libdodo/rpcXmlValue.h>
#include <libdodo/types.h>
#include <libdodo/dataFormatXmlNode.h>
#include <libdodo/toolsString.h>
#include <libdodo/rpcValue.h>

using namespace dodo::rpc::xml;

const char value::trimSymbols[] =
{
	' ',
	'\r'
};

//-------------------------------------------------------------------

dodo::rpc::value
value::xmlToValue(dodo::data::format::xml::node &node)
{
	dodoMap<dodoString, dodoArray<dodo::data::format::xml::node>, dodoMapStringCompare>::iterator i = node.children.begin();
	if (i == node.children.end())
		return rpc::value();

	rpc::value val;

	if (tools::string::iequal(i->first, "int") || tools::string::iequal(i->first, "i4"))
	{
		val.valueDataType = DATATYPE_INTEGER;

		dodoArray<dodo::data::format::xml::node> &arr0 = i->second;
		if (arr0.size() > 0)
			val.integerValue = tools::string::stringToI(tools::string::trim(arr0[0].value, trimSymbols, 2));
		else
			val.integerValue = 0;
	}
	else
	{
		if (tools::string::iequal(i->first, "boolean"))
		{
			val.valueDataType = DATATYPE_BOOLEAN;

			dodoArray<dodo::data::format::xml::node> &arr0 = i->second;
			if (arr0.size() > 0)
				val.booleanValue = tools::string::stringToI(tools::string::trim(arr0[0].value, trimSymbols, 2)) == 1 ? true : false;
			else
				val.booleanValue = false;
		}
		else
		{
			if (tools::string::iequal(i->first, "string") || tools::string::iequal(i->first, "base64") || tools::string::iequal(i->first, "dateTime.iso8601"))
			{
				val.valueDataType = DATATYPE_STRING;
				val.stringValue = new dodoString;

				dodoArray<dodo::data::format::xml::node> &arr0 = i->second;
				if (arr0.size() > 0)
					*val.stringValue = tools::string::trim(arr0[0].value, trimSymbols, 2);
			}
			else
			{
				if (tools::string::iequal(i->first, "double"))
				{
					val.valueDataType = DATATYPE_DOUBLE;

					dodoArray<dodo::data::format::xml::node> &arr0 = i->second;
					if (arr0.size() > 0)
						val.doubleValue = tools::string::stringToD(tools::string::trim(arr0[0].value, trimSymbols, 2));
					else
						val.doubleValue = 0;
				}
				else
				{
					if (tools::string::iequal(i->first, "struct"))
					{
						val.valueDataType = DATATYPE_STRUCT;
						val.structValue = new dodoMap<dodoString, rpc::value, dodoMapStringCompare>;

						dodoArray<dodo::data::format::xml::node> &arr0 = i->second;
						if (arr0.size() == 0)
							return val;

						dodoArray<dodo::data::format::xml::node> &nodeArray = arr0[0].children["member"];

						dodoArray<dodo::data::format::xml::node>::iterator o = nodeArray.begin(), p = nodeArray.end();
						for (; o != p; ++o)
						{
							dodoArray<dodo::data::format::xml::node> &arr1 = o->children["name"];
							dodoArray<dodo::data::format::xml::node> &arr2 = o->children["value"];
							if (arr1.size() > 0 && arr2.size() > 0)
								val.structValue->insert(make_pair(tools::string::trim(arr1[0].value, trimSymbols, 2), xmlToValue(arr2[0])));
						}
					}
					else
					{
						if (tools::string::iequal(i->first, "array"))
						{
							val.valueDataType = DATATYPE_ARRAY;
							val.arrayValue = new dodoArray<rpc::value>;

							dodoArray<dodo::data::format::xml::node> &arr0 = i->second;
							if (arr0.size() == 0)
								return val;

							dodoArray<dodo::data::format::xml::node> &arr1 = arr0[0].children["data"];
							if (arr1.size() == 0)
								return val;

							dodoArray<dodo::data::format::xml::node> &nodeArray = arr1[0].children["value"];

							dodoArray<dodo::data::format::xml::node>::iterator o = nodeArray.begin(), p = nodeArray.end();
							for (; o != p; ++o)
								val.arrayValue->push_back(xmlToValue(*o));
						}
					}
				}
			}
		}
	}

	return val;
}

//-------------------------------------------------------------------

dodo::data::format::xml::node
value::valueToXml(const rpc::value &data)
{
	dodoArray<dodo::data::format::xml::node> nodeArr;

	dodo::data::format::xml::node node;
	node.name = "value";

	dodo::data::format::xml::node subNode;

	switch (data.valueDataType)
	{
		case DATATYPE_STRING:

			subNode.name = "string";
			subNode.value = *data.stringValue;

			nodeArr.assign(1, subNode);
			node.children.insert(make_pair(subNode.name, nodeArr));

			break;

		case DATATYPE_BOOLEAN:

			subNode.name = "boolean";
			subNode.value = data.booleanValue ? "1" : "0";

			nodeArr.assign(1, subNode);
			node.children.insert(make_pair(subNode.name, nodeArr));

			break;

		case DATATYPE_INTEGER:

			subNode.name = "int";
			subNode.value = tools::string::iToString(data.integerValue);

			nodeArr.assign(1, subNode);
			node.children.insert(make_pair(subNode.name, nodeArr));

			break;

		case DATATYPE_DOUBLE:

			subNode.name = "double";
			subNode.value = tools::string::dToString(data.doubleValue);

			nodeArr.assign(1, subNode);
			node.children.insert(make_pair(subNode.name, nodeArr));

			break;

		case DATATYPE_ARRAY:
		{
			subNode.name = "array";

			dodo::data::format::xml::node dataNode;
			dataNode.name = "data";

			dodoArray<rpc::value>::const_iterator i = data.arrayValue->begin(), j = data.arrayValue->end();
			for (; i != j; ++i)
				nodeArr.push_back(valueToXml(*i));
			dataNode.children.insert(make_pair("value", nodeArr));

			nodeArr.assign(1, dataNode);
			subNode.children.insert(make_pair(dataNode.name, nodeArr));

			nodeArr.assign(1, subNode);
			node.children.insert(make_pair(subNode.name, nodeArr));

			break;
		}

		case DATATYPE_STRUCT:
		{
			subNode.name = "struct";

			dodo::data::format::xml::node memberNode, memberNameNode, memberValueNode;
			memberNode.name = "member";
			memberNameNode.name = "name";
			memberValueNode.name = "value";

			dodoArray<dodo::data::format::xml::node> subNodeArr;

			dodoMap<dodoString, rpc::value, dodoMapStringCompare>::const_iterator
				i = data.structValue->begin(),
				j = data.structValue->end();
			for (; i != j; ++i)
			{
				memberNode.children.clear();
				memberValueNode.children.clear();

				memberNameNode.value = i->first;
				nodeArr.assign(1, memberNameNode);
				memberNode.children.insert(make_pair(memberNameNode.name, nodeArr));

				nodeArr.assign(1, valueToXml(i->second));
				memberNode.children.insert(make_pair(memberValueNode.name, nodeArr));

				subNodeArr.push_back(memberNode);
			}
			subNode.children.insert(make_pair(memberNode.name, subNodeArr));

			nodeArr.assign(1, subNode);
			node.children.insert(make_pair(subNode.name, nodeArr));

			break;
		}
	}

	return node;
}

//-------------------------------------------------------------------
