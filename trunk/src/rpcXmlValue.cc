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

#include <libdodo/rpcXmlValue.h>

using namespace dodo;

const char rpcXmlValue::trimSymbols[] = {' ',
		'\r'
};

//-------------------------------------------------------------------

rpcValue 
rpcXmlValue::xmlToRpcValue(__xmlNode &node)
{
	rpcValue value;
	
	dodoMap<dodoString, dodoArray<__xmlNode>, dodoMapStringCompare>::iterator i = node.children.begin();
	if (stringTools::iequal(i->first, "int") || stringTools::iequal(i->first, "i4"))
	{
		value.valueDataType = RPC_DATATYPE_INTEGER;
		value.integerValue = stringTools::stringToI(stringTools::trim(i->second[0].value, trimSymbols, 2));
	}
	else
	{
		if (stringTools::iequal(i->first, "boolean"))
		{
			value.valueDataType = RPC_DATATYPE_BOOLEAN;
			value.booleanValue = stringTools::stringToI(stringTools::trim(i->second[0].value, trimSymbols, 2)) == 1?true:false;
		}
		else
		{
			if (stringTools::iequal(i->first, "string") || stringTools::iequal(i->first, "base64") || stringTools::iequal(i->first, "dateTime.iso8601"))
			{
				value.valueDataType = RPC_DATATYPE_STRING;
				value.stringValue = stringTools::trim(i->second[0].value, trimSymbols, 2);
			}
			else
			{
				if (stringTools::iequal(i->first, "double"))
				{
					value.valueDataType = RPC_DATATYPE_DOUBLE;
					value.doubleValue = stringTools::stringToD(stringTools::trim(i->second[0].value, trimSymbols, 2));
				}
				else
				{
					if (stringTools::iequal(i->first, "struct"))
					{
						value.valueDataType = RPC_DATATYPE_STRUCT;
						
						dodoArray<__xmlNode> &nodeArray = i->second[0].children["member"];
						
						dodoArray<__xmlNode>::iterator o = nodeArray.begin(), p = nodeArray.end();
						for (;o!=p;++o)
							value.structValue.insert(make_pair(stringTools::trim(o->children["name"][0].value, trimSymbols, 2), xmlToRpcValue(o->children["value"][0])));
					}
					else
					{
						if (stringTools::iequal(i->first, "array"))
						{
							value.valueDataType = RPC_DATATYPE_ARRAY;
							
							dodoArray<__xmlNode> &nodeArray = i->second[0].children["data"][0].children["value"];
							
							dodoArray<__xmlNode>::iterator o = nodeArray.begin(), p = nodeArray.end();
							for (;o!=p;++o)
								value.arrayValue.push_back(xmlToRpcValue(*o));
						}
					}
				}
			}
		}
	}
	
	return value;
}

//-------------------------------------------------------------------

rpcValue
rpcXmlValue::xmlToRpcValue(const dodoString &data)
{
	__xmlNodeDef xmlValueNode;
	xmlValueNode.name = "value";
	xmlValueNode.ignoreChildrenDef = true;
	
	xml xmlValue;

	__xmlNode node = xmlValue.parseBuffer(xmlValueNode, data);
	
	return xmlToRpcValue(node);
}

//-------------------------------------------------------------------

__xmlNode
rpcXmlValue::rpcValueToXmlNode(const rpcValue &data)
{
	dodoArray<__xmlNode> subNodeArr; 
	
	__xmlNode node;
	node.name = "value";
	
	__xmlNode subNode;
	
	switch (data.valueDataType)
	{
		case RPC_DATATYPE_STRING:
			
			subNode.name = "string";
			subNode.value = data.stringValue;
			
			subNodeArr.assign(1, subNode);
			node.children.insert(make_pair(subNode.name, subNodeArr));
			
			break;
			
		case RPC_DATATYPE_BOOLEAN:
			
			subNode.name = "boolean";
			subNode.value = data.booleanValue?"1":"0";

			subNodeArr.assign(1, subNode);
			node.children.insert(make_pair(subNode.name, subNodeArr));
			
			break;
			
		case RPC_DATATYPE_INTEGER:
			
			subNode.name = "int";
			subNode.value = stringTools::iToString(data.integerValue);

			subNodeArr.assign(1, subNode);
			node.children.insert(make_pair(subNode.name, subNodeArr));
		
			break;
			
		case RPC_DATATYPE_DOUBLE:
			
			subNode.name = "double";
			subNode.value = stringTools::dToString(data.doubleValue);

			subNodeArr.assign(1, subNode);
			node.children.insert(make_pair(subNode.name, subNodeArr));
			
			break;
			
		case RPC_DATATYPE_ARRAY:
		{
			subNode.name = "array";
			
			__xmlNode dataNode;
			dataNode.name = "data";
			
			dodoArray<rpcValue>::const_iterator i = data.arrayValue.begin(), j = data.arrayValue.end();
			for (;i!=j;++i)
				subNodeArr.push_back(rpcValueToXmlNode(*i));
			dataNode.children.insert(make_pair("value", subNodeArr));
			
			subNodeArr.assign(1, dataNode);
			subNode.children.insert(make_pair(dataNode.name, subNodeArr));
			
			subNodeArr.assign(1, subNode);
			node.children.insert(make_pair(subNode.name, subNodeArr));
			
			break;
		}
			
		case RPC_DATATYPE_STRUCT:
		{	
			subNode.name = "struct";
			
			__xmlNode memberNode, memberNameNode, memberValueNode, memberValueContentsNode;
			memberNode.name = "member";
			memberNameNode.name = "name";
			memberValueNode.name = "value";
			
			dodoMap<dodoString, rpcValue, dodoMapStringCompare>::const_iterator i = data.structValue.begin(), j = data.structValue.end();
			for (;i!=j;++i)
			{
				memberNode.children.clear();
				memberValueNode.children.clear();
				
				memberNameNode.value = i->first;
				subNodeArr.assign(1, memberNameNode);
				memberNode.children.insert(make_pair(memberNameNode.name, subNodeArr));

				memberValueContentsNode = rpcValueToXmlNode(i->second);
				subNodeArr.assign(1, memberValueContentsNode);
				memberValueNode.children.insert(make_pair(memberValueContentsNode.name, subNodeArr));
				subNodeArr.assign(1, memberValueNode);
				memberNode.children.insert(make_pair(memberValueNode.name, subNodeArr));
				
				subNodeArr.assign(1, memberNode);
				subNode.children.insert(make_pair(memberNode.name, subNodeArr));
			}
			
			subNodeArr.assign(1, subNode);
			node.children.insert(make_pair(subNode.name, subNodeArr));
			
			break;
		}
	}
	
	return node;
}

//-------------------------------------------------------------------

dodoString 
rpcXmlValue::rpcValueToXml(const rpcValue &data)
{
	xml xmlValue;
	
	return xmlValue.createNode(rpcValueToXmlNode(data));
}

//-------------------------------------------------------------------
