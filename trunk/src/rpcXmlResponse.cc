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

using namespace dodo;

const char rpcXmlResponse::trimSymbols[] = {' ',
		'\r'
};

//-------------------------------------------------------------------

rpcResponse
rpcXmlResponse::xmlToRpcResponse(const dodoString &data)
{
	__xmlNodeDef xmlMethodResponse;
	xmlMethodResponse.name = "methodResponse";
	xmlMethodResponse.ignoreChildrenDef = true;
	
	xml xmlValue;

	__xmlNode node = xmlValue.parseBuffer(xmlMethodResponse, data);
	
	return xmlToRpcResponse(node);
}

//-------------------------------------------------------------------

dodoString 
rpcXmlResponse::rpcResponseToXml(const rpcResponse &data)
{
	xml xmlValue;
	
	return xmlValue.createNode(rpcResponseToXmlNode(data));
}

//-------------------------------------------------------------------

rpcResponse
rpcXmlResponse::xmlToRpcResponse(__xmlNode &node)
{
	dodoMap<dodoString, dodoArray<__xmlNode>, dodoMapStringCompare>::iterator i = node.children.begin();
	if (i == node.children.end())
		return rpcResponse();

	rpcResponse response;
	
	if (stringTools::iequal(i->first, "fault"))
	{
		response.succ = false;
		
		dodoArray<__xmlNode> &arr0 = i->second;
		if (arr0.size() > 0)
		{
			dodoArray<__xmlNode> &arr1 = arr0[0].children["value"];
			if (arr1.size() > 0)
				response.values.assign(1, rpcXmlValue::xmlToRpcValue(arr1[0]));
		}
	}
	else
	{
		if (stringTools::iequal(i->first, "params"))
		{
			response.succ = true;
			
			dodoArray<__xmlNode> &arr0 = i->second;
			if (arr0.size() == 0)
				return response;
				
			dodoArray<__xmlNode> &nodeArray = arr0[0].children["param"];
			
			dodoArray<__xmlNode>::iterator o = nodeArray.begin(), p = nodeArray.end();
			for (;o!=p;++o)
			{
				dodoArray<__xmlNode> &arr1 = o->children["value"];
				if (arr1.size() > 0)
					response.values.push_back(rpcXmlValue::xmlToRpcValue(arr1[0]));
			}
		}
	}
	
	return response;
}

//-------------------------------------------------------------------

__xmlNode 
rpcXmlResponse::rpcResponseToXmlNode(const rpcResponse &data)
{
	dodoArray<__xmlNode> nodeArr; 
	
	__xmlNode response;
	response.name = "methodResponse";
	
	if (data.succ)
	{
		__xmlNode params;
		params.name = "params";
		
		__xmlNode param;
		param.name = "param";
		
		dodoArray<__xmlNode> subNodeArr; 
		
		dodoArray<rpcValue>::const_iterator i = data.values.begin(), j = data.values.end();
		for (;i!=j;++i)
		{
			param.children.clear();
			
			nodeArr.assign(1, rpcXmlValue::rpcValueToXmlNode(*i));
			param.children.insert(make_pair("value", nodeArr));
			
			subNodeArr.push_back(param);
		}
		params.children.insert(make_pair("param", subNodeArr));

		nodeArr.assign(1, params);
		response.children.insert(make_pair(params.name, nodeArr));
	}
	else
	{
		__xmlNode fault;
		fault.name = "fault";
		
		nodeArr.assign(1, rpcXmlValue::rpcValueToXmlNode(data.values.front()));
		fault.children.insert(make_pair("value", nodeArr));

		nodeArr.assign(1, fault);
		response.children.insert(make_pair(fault.name, nodeArr));
	}
	
	return response;
}

//-------------------------------------------------------------------
