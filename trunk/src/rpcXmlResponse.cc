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

#include <libdodo/rpcXmlResponse.h>

using namespace dodo::rpc::xml;

const char response::trimSymbols[] = {' ',
		'\r'
};

//-------------------------------------------------------------------

dodo::rpc::response
response::xmlToRpcResponse(const dodoString &data)
{
	__xmlNodeDef xmlMethodResponse;
	xmlMethodResponse.name = "methodResponse";
	xmlMethodResponse.ignoreChildrenDef = true;
	
	dodo::xml xmlValue;

	__xmlNode node = xmlValue.parseBuffer(xmlMethodResponse, data);
	
	return xmlToRpcResponse(node);
}

//-------------------------------------------------------------------

dodoString 
response::responseToXml(const rpc::response &data)
{
	dodo::xml xmlValue;
	
	return xmlValue.createNode(responseToXmlNode(data));
}

//-------------------------------------------------------------------

dodo::rpc::response
response::xmlToRpcResponse(__xmlNode &node)
{
	dodoMap<dodoString, dodoArray<__xmlNode>, dodoMapStringCompare>::iterator i = node.children.begin();
	if (i == node.children.end())
		return rpc::response();

	rpc::response resp;
	
	if (toolsString::iequal(i->first, "fault"))
	{
		resp.succ = false;
		
		dodoArray<__xmlNode> &arr0 = i->second;
		if (arr0.size() > 0)
		{
			dodoArray<__xmlNode> &arr1 = arr0[0].children["value"];
			if (arr1.size() > 0)
				resp.values.assign(1, value::xmlToRpcValue(arr1[0]));
		}
	}
	else
	{
		if (toolsString::iequal(i->first, "params"))
		{
			resp.succ = true;
			
			dodoArray<__xmlNode> &arr0 = i->second;
			if (arr0.size() == 0)
				return resp;
				
			dodoArray<__xmlNode> &nodeArray = arr0[0].children["param"];
			
			dodoArray<__xmlNode>::iterator o = nodeArray.begin(), p = nodeArray.end();
			for (;o!=p;++o)
			{
				dodoArray<__xmlNode> &arr1 = o->children["value"];
				if (arr1.size() > 0)
					resp.values.push_back(value::xmlToRpcValue(arr1[0]));
			}
		}
	}
	
	return resp;
}

//-------------------------------------------------------------------

dodo::__xmlNode 
response::responseToXmlNode(const rpc::response &data)
{
	dodoArray<__xmlNode> nodeArr; 
	
	__xmlNode resp;
	resp.name = "methodResponse";
	
	if (data.succ)
	{
		__xmlNode params;
		params.name = "params";
		
		__xmlNode param;
		param.name = "param";
		
		dodoArray<__xmlNode> subNodeArr; 
		
		dodoArray<rpc::value>::const_iterator i = data.values.begin(), j = data.values.end();
		for (;i!=j;++i)
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
		__xmlNode fault;
		fault.name = "fault";
		
		nodeArr.assign(1, value::valueToXmlNode(data.values.front()));
		fault.children.insert(make_pair("value", nodeArr));

		nodeArr.assign(1, fault);
		resp.children.insert(make_pair(fault.name, nodeArr));
	}
	
	return resp;
}

//-------------------------------------------------------------------
