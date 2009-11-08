/***************************************************************************
 *            rpcJsonValue.cc
 *
 *  Mon Jul 07 2008
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

#include <libdodo/rpcJsonValue.h>
#include <libdodo/types.h>
#include <libdodo/dataFormatJsonNode.h>
#include <libdodo/rpcValue.h>

using namespace dodo::rpc::json;

dodo::rpc::value
value::jsonToValue(const dodo::data::format::json::node &node)
{
    rpc::value val;

    switch (node.valueDataType) {
        case dodo::data::format::json::node::DATA_STRING:
            val.valueDataType = dodo::rpc::value::DATA_STRING;
            val.stringValue = new dodoString(*node.stringValue);

            break;

        case dodo::data::format::json::node::DATA_OBJECT:
        {
            val.valueDataType = dodo::rpc::value::DATA_STRUCT;
            val.structValue = new dodoMap<dodoString, rpc::value, dodoMapStringCompare>;

            dodoMap<dodoString, dodo::data::format::json::node, dodoMapStringCompare>::iterator
            i = node.objectValue->begin(),
            j = node.objectValue->end();
            for (; i != j; ++i)
                val.structValue->insert(make_pair(i->first, jsonToValue(i->second)));

            break;
        }

        case dodo::data::format::json::node::DATA_ARRAY:
        {
            val.valueDataType = dodo::rpc::value::DATA_ARRAY;
            val.arrayValue = new dodoArray<rpc::value>;

            dodoArray<dodo::data::format::json::node>::const_iterator
            i = node.arrayValue->begin(),
            j = node.arrayValue->end();
            for (; i != j; ++i)
                val.arrayValue->push_back(jsonToValue(*i));

            break;
        }

        case dodo::data::format::json::node::DATA_BOOLEAN:
            val.valueDataType = dodo::rpc::value::DATA_BOOLEAN;
            val.booleanValue = node.booleanValue;

            break;

        case dodo::data::format::json::node::DATA_NUMERIC:
            val.valueDataType = dodo::rpc::value::DATA_NUMERIC;
            val.numericValue = node.numericValue;

            break;

        case dodo::data::format::json::node::DATA_NULL:
            val.valueDataType = dodo::rpc::value::DATA_BOOLEAN;
            val.booleanValue = false;

            break;
    }

    return val;
}

//-------------------------------------------------------------------

dodo::data::format::json::node
value::valueToJson(const rpc::value &data)
{
    dodo::data::format::json::node node;

    switch (data.valueDataType) {
        case dodo::rpc::value::DATA_STRING:
            node.valueDataType = dodo::data::format::json::node::DATA_STRING;
            node.stringValue = new dodoString(*data.stringValue);

            break;

        case dodo::rpc::value::DATA_BOOLEAN:
            node.valueDataType = dodo::data::format::json::node::DATA_BOOLEAN;
            node.booleanValue = data.booleanValue;

            break;

        case dodo::rpc::value::DATA_NUMERIC:
            node.valueDataType = dodo::data::format::json::node::DATA_NUMERIC;
            node.numericValue = (long)data.numericValue;

            break;

        case dodo::rpc::value::DATA_ARRAY:
        {
            node.valueDataType = dodo::data::format::json::node::DATA_ARRAY;
            node.arrayValue = new dodoArray<dodo::data::format::json::node>;

            dodoArray<rpc::value>::const_iterator
            i = data.arrayValue->begin(),
            j = data.arrayValue->end();
            for (; i != j; ++i)
                node.arrayValue->push_back(valueToJson(*i));

            break;
        }

        case dodo::rpc::value::DATA_STRUCT:
        {
            node.valueDataType = dodo::data::format::json::node::DATA_OBJECT;
            node.objectValue = new dodoMap<dodoString, dodo::data::format::json::node, dodoMapStringCompare>;

            dodoMap<dodoString, rpc::value, dodoMapStringCompare>::const_iterator
            i = data.structValue->begin(),
            j = data.structValue->end();
            for (; i != j; ++i)
                node.objectValue->insert(make_pair(i->first, valueToJson(i->second)));

            break;
        }
    }

    return node;
}

//-------------------------------------------------------------------
