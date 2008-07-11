/***************************************************************************
 *            rpcJsonMethod.cc
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

#include <libdodo/rpcJsonMethod.h>

using namespace dodo::rpc::json;

dodo::rpc::method
method::jsonToRpcMethod(const dodoString &data)
{
	dodo::json::processor jsonValue;

	dodo::json::node node = jsonValue.process(data);

	return jsonToRpcMethod(node);
}

//-------------------------------------------------------------------

dodoString
method::methodToJson(const rpc::method &data)
{
	dodo::json::processor jsonValue;

	return jsonValue.make(methodToJsonNode(data));
}

//-------------------------------------------------------------------


dodo::rpc::method
method::jsonToRpcMethod(dodo::json::node &node)
{
	dodoMap<dodoString, dodo::json::node, dodoMapStringCompare> obj = node.getObject();
	
	rpc::method meth;

	meth.name = obj["method"].getString();

	dodoArray<dodo::json::node> nodes = obj["params"].getArray();
	
	dodoArray<dodo::json::node>::iterator i = nodes.begin(), j = nodes.end();
	for (;i!=j;++i)
		meth.arguments.push_back(value::jsonToRpcValue(*i));

	return meth;
}

//-------------------------------------------------------------------

dodo::json::node
method::methodToJsonNode(const rpc::method &data)
{
	/*dodoArray<dodo::json::node> nodeArr;

	dodo::json::node meth;
	meth.name = "methodCall";

	dodo::json::node methodName;
	methodName.name = "methodName";
	methodName.value = data.name;

	nodeArr.assign(1, methodName);
	meth.children.insert(make_pair(methodName.name, nodeArr));

	dodoArray<rpc::value>::const_iterator i = data.arguments.begin(), j = data.arguments.end();
	if (i != j)
	{
		dodo::json::node params;
		params.name = "params";

		dodo::json::node param;
		param.name = "param";

		dodoArray<dodo::json::node> subNodeArr;

		for (; i != j; ++i)
		{
			param.children.clear();

			nodeArr.assign(1, value::valueToJsonNode(*i));
			param.children.insert(make_pair("value", nodeArr));

			subNodeArr.push_back(param);
		}
		params.children.insert(make_pair("param", subNodeArr));

		nodeArr.assign(1, params);
		meth.children.insert(make_pair(params.name, nodeArr));
	}

	return meth;*/
}

//-------------------------------------------------------------------
