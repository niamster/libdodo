/***************************************************************************
 *            rpcJsonMethod.cc
 *
 *  Mon Jul 07 2008
 *  Copyright  2008  Dmytro Milinevskyy
 *  milinevskyy@gmail.com
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

#include <libdodo/rpcJsonMethod.h>
#include <libdodo/types.h>
#include <libdodo/dataFormatJsonNode.h>
#include <libdodo/rpcMethod.h>
#include <libdodo/rpcJsonMethodEx.h>
#include <libdodo/rpcJsonValue.h>
#include <libdodo/rpcValue.h>

using namespace dodo::rpc::json;

dodo::rpc::method
method::jsonToMethod(dodo::data::format::json::node &node,
                     dodo::string                     &version,
                     long                           &id)
{
    if (node.valueDataType != dodo::data::format::json::node::DATA_OBJECT)
        throw exception::basic(exception::MODULE_RPCJSONMETHOD, METHODEX_JSONTOMETHOD, exception::ERRNO_LIBDODO, METHODEX_ROOTNOTANOBJECT, RPCJSONMETHODEX_ROOTNOTANOBJECT_STR, __LINE__, __FILE__);

    rpc::method meth;

    meth.name = (*node.objectValue)["method"].string();
    version = (*node.objectValue)["version"].string();
    id = (*node.objectValue)["id"].numeric();

    dodo::data::format::json::node &params = (*node.objectValue)["params"];

    if (params.valueDataType != dodo::data::format::json::node::DATA_ARRAY &&
        params.valueDataType != dodo::data::format::json::node::DATA_OBJECT)
        throw exception::basic(exception::MODULE_RPCJSONMETHOD, METHODEX_JSONTOMETHOD, exception::ERRNO_LIBDODO, METHODEX_PARAMSNOTANARRAY, RPCJSONMETHODEX_PARAMSNOTANARRAY_STR, __LINE__, __FILE__);

    if (params.valueDataType == dodo::data::format::json::node::DATA_ARRAY) {
        dodoArray<dodo::data::format::json::node>::iterator
        i = params.arrayValue->begin(),
        j = params.arrayValue->end();
        for (; i != j; ++i)
            meth.arguments.push_back(value::jsonToValue(*i));
    } else
        meth.arguments.push_back(value::jsonToValue(params));

    return meth;
}

//-------------------------------------------------------------------

dodo::data::format::json::node
method::methodToJson(const rpc::method &data,
                     const dodo::string  &version,
                     long              id)
{
    dodo::data::format::json::node meth;
    dodo::data::format::json::node node;

    meth.valueDataType = dodo::data::format::json::node::DATA_OBJECT;
    meth.objectValue = new dodoMap<dodo::string, dodo::data::format::json::node, dodoMapStringCompare>;

    node.valueDataType = dodo::data::format::json::node::DATA_STRING;

    node.stringValue = new dodo::string(data.name);
    meth.objectValue->insert(std::make_pair(dodo::string("method"), node));
    delete node.stringValue;

    node.stringValue = new dodo::string(version);
    meth.objectValue->insert(std::make_pair(dodo::string("version"), node));
    delete node.stringValue;

    node.valueDataType = dodo::data::format::json::node::DATA_NUMERIC;
    node.numericValue = id;
    meth.objectValue->insert(std::make_pair(dodo::string("id"), node));

    dodoArray<rpc::value>::const_iterator i = data.arguments.begin(), j = data.arguments.end();
    if (i != j) {
        if (data.arguments.size() == 1 && i->valueDataType == dodo::rpc::value::DATA_STRUCT)
            meth.objectValue->insert(std::make_pair(dodo::string("params"), value::valueToJson(*i)));
        else {
            node.valueDataType = dodo::data::format::json::node::DATA_ARRAY;
            node.arrayValue = new dodoArray<dodo::data::format::json::node>;

            for (; i != j; ++i)
                node.arrayValue->push_back(value::valueToJson(*i));

            meth.objectValue->insert(std::make_pair(dodo::string("params"), node));

            delete node.arrayValue;
            node.valueDataType = dodo::data::format::json::node::DATA_NULL;
        }
    }

    return meth;
}

//-------------------------------------------------------------------
