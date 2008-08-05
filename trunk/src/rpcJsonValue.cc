/***************************************************************************
 *            rpcJsonValue.cc
 *
 *  Mon Jul 07 11:29:55 2008
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

#include <libdodo/rpcJsonValue.h>

using namespace dodo::rpc::json;

dodo::rpc::value
value::jsonToValue(dodo::data::format::json::node &node)
{
	rpc::value val;

	switch (node.valueDataType)
	{
		case dodo::data::format::json::DATATYPE_STRING:

			val.valueDataType = DATATYPE_STRING;

			val.stringValue = node.stringValue;

			break;

		case dodo::data::format::json::DATATYPE_OBJECT:
		{
			val.valueDataType = DATATYPE_STRUCT;

			dodoMap<dodoString, dodo::data::format::json::node, dodoMapStringCompare>::iterator i = node.objectValue.begin(), j = node.objectValue.end();
			for (; i != j; ++i)
				val.structValue.insert(make_pair(i->first, jsonToValue(i->second)));

			break;
		}

		case dodo::data::format::json::DATATYPE_ARRAY:
		{
			val.valueDataType = DATATYPE_ARRAY;

			dodoArray<dodo::data::format::json::node>::iterator i = node.arrayValue.begin(), j = node.arrayValue.end();
			for (; i != j; ++i)
				val.arrayValue.push_back(jsonToValue(*i));

			break;
		}

		case dodo::data::format::json::DATATYPE_BOOLEAN:

			val.valueDataType = DATATYPE_BOOLEAN;

			val.booleanValue = node.booleanValue;

			break;

		case dodo::data::format::json::DATATYPE_NUMERIC:

			val.valueDataType = DATATYPE_INTEGER;

			val.integerValue = node.numericValue;

			break;

		case dodo::data::format::json::DATATYPE_NULL:

			val.valueDataType = DATATYPE_BOOLEAN;

			val.booleanValue = false;

			break;
	}

	return val;
}

//-------------------------------------------------------------------

dodo::data::format::json::node
value::valueToJson(const rpc::value &data)
{
	dodo::data::format::json::node node;

	switch (data.valueDataType)
	{
		case DATATYPE_STRING:

			node.valueDataType = dodo::data::format::json::DATATYPE_STRING;

			node.stringValue = data.stringValue;

			break;

		case DATATYPE_BOOLEAN:

			node.valueDataType = dodo::data::format::json::DATATYPE_BOOLEAN;

			node.booleanValue = data.booleanValue;

			break;

		case DATATYPE_INTEGER:

			node.valueDataType = dodo::data::format::json::DATATYPE_NUMERIC;

			node.numericValue = data.integerValue;

			break;

		case DATATYPE_DOUBLE:

			node.valueDataType = dodo::data::format::json::DATATYPE_NUMERIC;

			node.numericValue = (long)data.doubleValue;

			break;

		case DATATYPE_ARRAY:
		{
			node.valueDataType = dodo::data::format::json::DATATYPE_ARRAY;

			dodoArray<rpc::value>::const_iterator i = data.arrayValue.begin(), j = data.arrayValue.end();
			for (; i != j; ++i)
				node.arrayValue.push_back(valueToJson(*i));

			break;
		}

		case DATATYPE_STRUCT:
		{
			node.valueDataType = dodo::data::format::json::DATATYPE_OBJECT;

			dodoMap<dodoString, rpc::value, dodoMapStringCompare>::const_iterator i = data.structValue.begin(), j = data.structValue.end();
			for (; i != j; ++i)
				node.objectValue.insert(make_pair(i->first, valueToJson(i->second)));

			break;
		}
	}

	return node;
}

//-------------------------------------------------------------------
