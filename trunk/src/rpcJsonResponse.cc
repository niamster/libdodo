/***************************************************************************
 *            rpcJsonResponse.cc
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

#include <libdodo/rpcJsonResponse.h>

using namespace dodo::rpc::json;

dodo::rpc::response
response::jsonToResponse(dodo::data::format::json::node &node,
						 dodoString &version,
						 long &id)
{
	if (node.valueDataType != dodo::data::format::json::DATATYPE_OBJECT)
		throw exception::basic(exception::ERRMODULE_RPCJSONRESPONSE, RESPONSEEX_JSONTORESPONSE, exception::ERRNO_LIBDODO, RESPONSEEX_ROOTNOTANOBJECT, RPCJSONRESPONSEEX_ROOTNOTANOBJECT_STR, __LINE__, __FILE__);

	dodoMap<dodoString, dodo::data::format::json::node, dodoMapStringCompare> &obj = node.objectValue;

	rpc::response resp;

	version = obj["version"].getString();

	id = obj["id"].getNumeric();

	dodo::data::format::json::node &error = obj["error"];

	if (!error.isNull())
	{
		resp.values.push_back(value::jsonToValue(error));
	}
	else
	{
		resp.succ = true;

		dodo::data::format::json::node &result = obj["result"];

		if (result.valueDataType == dodo::data::format::json::DATATYPE_ARRAY)
		{
			dodoArray<dodo::data::format::json::node>::iterator i = result.arrayValue.begin(), j = result.arrayValue.end();
			for (; i != j; ++i)
				resp.values.push_back(value::jsonToValue(*i));
		}
		else
			resp.values.push_back(value::jsonToValue(result));
	}

	return resp;
}

//-------------------------------------------------------------------

dodo::data::format::json::node
response::responseToJson(const rpc::response &data,
						 const dodoString &version,
						 long id)
{
	dodo::data::format::json::node resp;

	resp.valueDataType = dodo::data::format::json::DATATYPE_OBJECT;

	dodo::data::format::json::node node;

	node.valueDataType = dodo::data::format::json::DATATYPE_STRING;
	node.stringValue = version;
	resp.objectValue.insert(make_pair(dodoString("version"), node));

	node.stringValue.clear();

	node.valueDataType = dodo::data::format::json::DATATYPE_NUMERIC;
	node.numericValue = id;
	resp.objectValue.insert(make_pair(dodoString("id"), node));

	dodoArray<rpc::value>::const_iterator i = data.values.begin(), j = data.values.end();
	if (i != j)
	{
		if (!data.succ)
			resp.objectValue.insert(make_pair(dodoString("error"), value::valueToJson(*i)));
		else
		{
			if (data.values.size() == 1)
				resp.objectValue.insert(make_pair(dodoString("result"), value::valueToJson(*i)));
			else
			{
				dodo::data::format::json::node subNode;
				subNode.valueDataType = dodo::data::format::json::DATATYPE_ARRAY;

				for (; i != j; ++i)
					subNode.arrayValue.push_back(value::valueToJson(*i));

				resp.objectValue.insert(make_pair(dodoString("result"), subNode));
			}
		}
	}

	return resp;
}

//-------------------------------------------------------------------

