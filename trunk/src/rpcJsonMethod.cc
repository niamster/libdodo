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
method::jsonToMethod(dodo::json::node &node,
					 dodoString &version)
{
	if (node.valueDataType != dodo::json::DATATYPE_OBJECT)
		throw baseEx(ERRMODULE_RPCJSONMETHOD, METHODEX_JSONTOMETHOD, ERR_LIBDODO, METHODEX_ROOTNOTANOBJECT, RPCJSONMETHODEX_ROOTNOTANOBJECT_STR, __LINE__, __FILE__);
	
	dodoMap<dodoString, dodo::json::node, dodoMapStringCompare> &obj = node.objectValue;
		
	rpc::method meth;

	meth.name = obj["method"].getString();

	version = obj["version"].getString();

	dodo::json::node &params = obj["params"];

	if (params.valueDataType != dodo::json::DATATYPE_ARRAY && params.valueDataType != dodo::json::DATATYPE_OBJECT)
		throw baseEx(ERRMODULE_RPCJSONMETHOD, METHODEX_JSONTOMETHOD, ERR_LIBDODO, METHODEX_PARAMSNOTANARRAY, RPCJSONMETHODEX_PARAMSNOTANARRAY_STR, __LINE__, __FILE__);
	
	if (params.valueDataType == dodo::json::DATATYPE_ARRAY)
	{
		dodoArray<dodo::json::node>::iterator i = params.arrayValue.begin(), j = params.arrayValue.end();
		for (;i!=j;++i)
			meth.arguments.push_back(value::jsonToValue(*i));
	}
	else
		meth.arguments.push_back(value::jsonToValue(params));


	return meth;
}

//-------------------------------------------------------------------

dodo::json::node
method::methodToJson(const rpc::method &data,
					 const dodoString &version)
{
	dodo::json::node meth;

	meth.valueDataType = dodo::json::DATATYPE_OBJECT;

	dodo::json::node node;
	
	node.valueDataType = dodo::json::DATATYPE_STRING;	

	node.stringValue = data.name;
	meth.objectValue.insert(make_pair(dodoString("method"), node));
	
	node.stringValue = version;
	meth.objectValue.insert(make_pair(dodoString("version"), node));

	dodoArray<rpc::value>::const_iterator i = data.arguments.begin(), j = data.arguments.end();
	if (i != j)
	{
		if (data.arguments.size() == 1 && i->valueDataType == DATATYPE_STRUCT)
			meth.objectValue.insert(make_pair(dodoString("params"), value::valueToJson(*i)));
		else
		{
			node.stringValue.clear();

			node.valueDataType = dodo::json::DATATYPE_ARRAY;

			for (;i!=j;++i)
				node.arrayValue.push_back(value::valueToJson(*i));	
		
			meth.objectValue.insert(make_pair(dodoString("params"), node));
		}
		
	}

	return meth;
}

//-------------------------------------------------------------------
