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
response::jsonToResponse(dodo::json::node &node,
						 dodoString &version)
{
	rpc::response resp;

	

	/*dodoMap<dodoString, dodoArray<dodo::json::node>, dodoMapStringCompare>::iterator i = node.children.begin();
	if (i == node.children.end())
		return rpc::response();

	rpc::response resp;

	if (tools::string::iequal(i->first, "fault"))
	{
		resp.succ = false;

		dodoArray<dodo::json::node> &arr0 = i->second;
		if (arr0.size() > 0)
		{
			dodoArray<dodo::json::node> &arr1 = arr0[0].children["value"];
			if (arr1.size() > 0)
				resp.values.assign(1, value::jsonToValue(arr1[0]));
		}
	}
	else
	{
		if (tools::string::iequal(i->first, "params"))
		{
			resp.succ = true;

			dodoArray<dodo::json::node> &arr0 = i->second;
			if (arr0.size() == 0)
				return resp;

			dodoArray<dodo::json::node> &nodeArray = arr0[0].children["param"];

			dodoArray<dodo::json::node>::iterator o = nodeArray.begin(), p = nodeArray.end();
			for (; o != p; ++o)
			{
				dodoArray<dodo::json::node> &arr1 = o->children["value"];
				if (arr1.size() > 0)
					resp.values.push_back(value::jsonToValue(arr1[0]));
			}
		}
	}*/

	return resp;
}

//-------------------------------------------------------------------

dodo::json::node
response::responseToJson(const rpc::response &data,
						 const dodoString &version)
{
	dodo::json::node resp;

	/*dodoArray<dodo::json::node> nodeArr;

	resp.name = "methodResponse";

	if (data.succ)
	{
		dodo::json::node params;
		params.name = "params";

		dodo::json::node param;
		param.name = "param";

		dodoArray<dodo::json::node> subNodeArr;

		dodoArray<rpc::value>::const_iterator i = data.values.begin(), j = data.values.end();
		for (; i != j; ++i)
		{
			param.children.clear();

			nodeArr.assign(1, value::valueToJson(*i));
			param.children.insert(make_pair("value", nodeArr));

			subNodeArr.push_back(param);
		}
		params.children.insert(make_pair("param", subNodeArr));

		nodeArr.assign(1, params);
		resp.children.insert(make_pair(params.name, nodeArr));
	}
	else
	{
		dodo::json::node fault;
		fault.name = "fault";

		nodeArr.assign(1, value::valueToJson(data.values.front()));
		fault.children.insert(make_pair("value", nodeArr));

		nodeArr.assign(1, fault);
		resp.children.insert(make_pair(fault.name, nodeArr));
	}*/

	return resp;
}

//-------------------------------------------------------------------
