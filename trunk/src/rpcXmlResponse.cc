/***************************************************************************
 *            rpcXmlResponse.cc
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

#include <libdodo/rpcXmlResponse.h>

using namespace dodo::rpc::xml;

dodo::rpc::response
response::xmlToRpcResponse(const dodoString &data)
{
	dodo::xml::__nodeDef xmlMethodResponse;
	xmlMethodResponse.name = "methodResponse";
	xmlMethodResponse.ignoreChildrenDef = true;

	dodo::xml::processor xmlValue;

	dodo::xml::node node = xmlValue.processBuffer(xmlMethodResponse, data);

	return xmlToRpcResponse(node);
}

//-------------------------------------------------------------------

dodoString
response::responseToXml(const rpc::response &data)
{
	dodo::xml::processor xmlValue;

	return xmlValue.make(responseToXmlNode(data));
}

//-------------------------------------------------------------------

dodo::rpc::response
response::xmlToRpcResponse(dodo::xml::node &node)
{
	dodoMap<dodoString, dodoArray<dodo::xml::node>, dodoMapStringCompare>::iterator i = node.children.begin();
	if (i == node.children.end())
		return rpc::response();

	rpc::response resp;

	if (tools::string::iequal(i->first, "fault"))
	{
		resp.succ = false;

		dodoArray<dodo::xml::node> &arr0 = i->second;
		if (arr0.size() > 0)
		{
			dodoArray<dodo::xml::node> &arr1 = arr0[0].children["value"];
			if (arr1.size() > 0)
				resp.values.assign(1, value::xmlToRpcValue(arr1[0]));
		}
	}
	else
	{
		if (tools::string::iequal(i->first, "params"))
		{
			resp.succ = true;

			dodoArray<dodo::xml::node> &arr0 = i->second;
			if (arr0.size() == 0)
				return resp;

			dodoArray<dodo::xml::node> &nodeArray = arr0[0].children["param"];

			dodoArray<dodo::xml::node>::iterator o = nodeArray.begin(), p = nodeArray.end();
			for (; o != p; ++o)
			{
				dodoArray<dodo::xml::node> &arr1 = o->children["value"];
				if (arr1.size() > 0)
					resp.values.push_back(value::xmlToRpcValue(arr1[0]));
			}
		}
	}

	return resp;
}

//-------------------------------------------------------------------

dodo::xml::node
response::responseToXmlNode(const rpc::response &data)
{
	dodoArray<dodo::xml::node> nodeArr;

	dodo::xml::node resp;
	resp.name = "methodResponse";

	if (data.succ)
	{
		dodo::xml::node params;
		params.name = "params";

		dodo::xml::node param;
		param.name = "param";

		dodoArray<dodo::xml::node> subNodeArr;

		dodoArray<rpc::value>::const_iterator i = data.values.begin(), j = data.values.end();
		for (; i != j; ++i)
		{
			param.children.clear();

			nodeArr.assign(1, value::valueToXmlNode(*i));
			param.children.insert(make_pair("value", nodeArr));

			subNodeArr.push_back(param);
		}
		params.children.insert(make_pair("param", subNodeArr));

		nodeArr.assign(1, params);
		resp.children.insert(make_pair(params.name, nodeArr));
	}
	else
	{
		dodo::xml::node fault;
		fault.name = "fault";

		nodeArr.assign(1, value::valueToXmlNode(data.values.front()));
		fault.children.insert(make_pair("value", nodeArr));

		nodeArr.assign(1, fault);
		resp.children.insert(make_pair(fault.name, nodeArr));
	}

	return resp;
}

//-------------------------------------------------------------------
