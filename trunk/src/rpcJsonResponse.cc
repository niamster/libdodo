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

#include <libdodo/directives.h>

#include <libdodo/rpcJsonResponse.h>
#include <libdodo/types.h>
#include <libdodo/dataFormatJsonNode.h>
#include <libdodo/rpcResponse.h>
#include <libdodo/rpcJsonResponseEx.h>
#include <libdodo/rpcJsonValue.h>

using namespace dodo::rpc::json;

dodo::rpc::response
response::jsonToResponse(dodo::data::format::json::node &node,
						 dodoString                     &version,
						 long                           &id)
{
	if (node.valueDataType != dodo::data::format::json::DATATYPE_OBJECT)
		throw exception::basic(exception::ERRMODULE_RPCJSONRESPONSE, RESPONSEEX_JSONTORESPONSE, exception::ERRNO_LIBDODO, RESPONSEEX_ROOTNOTANOBJECT, RPCJSONRESPONSEEX_ROOTNOTANOBJECT_STR, __LINE__, __FILE__);

	rpc::response resp;

	version = (*node.objectValue)["version"].getString();
	id = (*node.objectValue)["id"].getNumeric();

	const dodo::data::format::json::node &error = (*node.objectValue)["error"];

	if (!error.isNull())
		resp.values.push_back(value::jsonToValue(error));
	else {
		resp.succ = true;

		const dodo::data::format::json::node &result = (*node.objectValue)["result"];

		if (result.valueDataType == dodo::data::format::json::DATATYPE_ARRAY) {
			dodoArray<dodo::data::format::json::node>::const_iterator
			i = result.arrayValue->begin(),
			j = result.arrayValue->end();
			for (; i != j; ++i)
				resp.values.push_back(value::jsonToValue(*i));
		} else
			resp.values.push_back(value::jsonToValue(result));
	}

	return resp;
}

//-------------------------------------------------------------------

dodo::data::format::json::node
response::responseToJson(const rpc::response &data,
						 const dodoString    &version,
						 long                id)
{
	dodo::data::format::json::node resp;
	dodo::data::format::json::node node;

	resp.valueDataType = dodo::data::format::json::DATATYPE_OBJECT;
	resp.objectValue = new dodoMap<dodoString, dodo::data::format::json::node, dodoMapStringCompare>;

	node.valueDataType = dodo::data::format::json::DATATYPE_STRING;

	node.stringValue = new dodoString(version);
	resp.objectValue->insert(make_pair(dodoString("version"), node));
	delete node.stringValue;

	node.valueDataType = dodo::data::format::json::DATATYPE_NUMERIC;
	node.numericValue = id;
	resp.objectValue->insert(make_pair(dodoString("id"), node));

	dodoArray<rpc::value>::const_iterator i = data.values.begin(), j = data.values.end();
	if (i != j) {
		if (!data.succ)
			resp.objectValue->insert(make_pair(dodoString("error"), value::valueToJson(*i)));
		else {
			if (data.values.size() == 1)
				resp.objectValue->insert(make_pair(dodoString("result"), value::valueToJson(*i)));
			else {
				node.valueDataType = dodo::data::format::json::DATATYPE_ARRAY;
				node.arrayValue = new dodoArray<dodo::data::format::json::node>;

				for (; i != j; ++i)
					node.arrayValue->push_back(value::valueToJson(*i));

				resp.objectValue->insert(make_pair(dodoString("result"), node));

				delete node.arrayValue;
				node.valueDataType = dodo::data::format::json::DATATYPE_NULL;
			}
		}
	}

	return resp;
}

//-------------------------------------------------------------------

