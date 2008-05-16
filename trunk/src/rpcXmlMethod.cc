/***************************************************************************
 *            rpcXmlMethod.cc
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

#include <libdodo/rpcXmlMethod.h>

using namespace dodo::rpc::xml;

const char method::trimSymbols[] = { ' ',
									 '\r' };

//-------------------------------------------------------------------

dodo::rpc::method
method::xmlToRpcMethod(const dodoString &data)
{
	dodo::xml::__nodeDef xmlMethodCall;
	xmlMethodCall.name = "methodCall";
	xmlMethodCall.ignoreChildrenDef = true;

	dodo::xml::processor xmlValue;

	dodo::xml::node node = xmlValue.parseBuffer(xmlMethodCall, data);

	return xmlToRpcMethod(node);
}

//-------------------------------------------------------------------

dodoString
method::methodToXml(const rpc::method &data)
{
	dodo::xml::processor xmlValue;

	return xmlValue.createNode(methodToXmlNode(data));
}

//-------------------------------------------------------------------


dodo::rpc::method
method::xmlToRpcMethod(dodo::xml::node &node)
{
	dodoMap<dodoString, dodoArray<dodo::xml::node>, dodoMapStringCompare>::iterator i = node.children.begin();
	if (i == node.children.end())
		return rpc::method();

	rpc::method meth;

	if (tools::string::iequal(i->first, "methodName"))
	{
		dodoArray<dodo::xml::node> &arr0 = i->second;
		if (arr0.size() > 0)
			meth.name = tools::string::trim(arr0[0].value, trimSymbols, 2);
		else
			meth.name = __dodostring__;
	}
	else
	{
		if (tools::string::iequal(i->first, "params"))
		{
			dodoArray<dodo::xml::node> &arr0 = i->second;
			if (arr0.size() == 0)
				return meth;

			dodoArray<dodo::xml::node> &nodeArray = arr0[0].children["param"];

			dodoArray<dodo::xml::node>::iterator o = nodeArray.begin(), p = nodeArray.end();
			for (; o != p; ++o)
			{
				dodoArray<dodo::xml::node> &arr1 = o->children["value"];
				if (arr0.size() > 0)
					meth.arguments.push_back(value::xmlToRpcValue(arr1[0]));
			}
		}
	}

	return meth;
}

//-------------------------------------------------------------------

dodo::xml::node
method::methodToXmlNode(const rpc::method &data)
{
	dodoArray<dodo::xml::node> nodeArr;

	dodo::xml::node meth;
	meth.name = "methodCall";

	dodo::xml::node methodName;
	methodName.name = "methodName";
	methodName.value = data.name;

	nodeArr.assign(1, methodName);
	meth.children.insert(make_pair(methodName.name, nodeArr));

	dodoArray<rpc::value>::const_iterator i = data.arguments.begin(), j = data.arguments.end();
	if (i != j)
	{
		dodo::xml::node params;
		params.name = "params";

		dodo::xml::node param;
		param.name = "param";

		dodoArray<dodo::xml::node> subNodeArr;

		for (; i != j; ++i)
		{
			param.children.clear();

			nodeArr.assign(1, value::valueToXmlNode(*i));
			param.children.insert(make_pair("value", nodeArr));

			subNodeArr.push_back(param);
		}
		params.children.insert(make_pair("param", subNodeArr));

		nodeArr.assign(1, params);
		meth.children.insert(make_pair(params.name, nodeArr));
	}

	return meth;
}

//-------------------------------------------------------------------
