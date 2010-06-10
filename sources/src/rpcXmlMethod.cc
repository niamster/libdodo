/***************************************************************************
 *            rpcXmlMethod.cc
 *
 *  Wed Apr 09 2008
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

#include <libdodo/rpcXmlMethod.h>
#include <libdodo/types.h>
#include <libdodo/dataFormatXmlNode.h>
#include <libdodo/toolsString.h>
#include <libdodo/rpcMethod.h>
#include <libdodo/rpcValue.h>
#include <libdodo/rpcXmlValue.h>

using namespace dodo::rpc::xml;

const char method::trimSymbols[] = {
    ' ',
    '\r'
};

//-------------------------------------------------------------------

dodo::rpc::method
method::xmlToMethod(dodo::data::format::xml::node &node)
{
    rpc::method meth;

    dodoMap<dodo::string, dodoArray<dodo::data::format::xml::node>, dodoMapStringCompare>::iterator i = node.nodeChildren.begin(), j = node.nodeChildren.end();
    for (; i != j; ++i) {
        if (tools::string::iequal(i->first, "methodName")) {
            dodoArray<dodo::data::format::xml::node> &arr0 = i->second;
            if (arr0.size() > 0)
                meth.name = tools::string::trim(arr0[0].nodeValue, trimSymbols, 2);
            else
                meth.name = __dodostring__;
        } else {
            if (tools::string::iequal(i->first, "params")) {
                dodoArray<dodo::data::format::xml::node> &arr0 = i->second;
                if (arr0.size() == 0)
                    return meth;

                dodoArray<dodo::data::format::xml::node> &nodeArray = arr0[0].nodeChildren["param"];

                dodoArray<dodo::data::format::xml::node>::iterator o = nodeArray.begin(), p = nodeArray.end();
                for (; o != p; ++o) {
                    dodoArray<dodo::data::format::xml::node> &arr1 = o->nodeChildren["value"];
                    if (arr0.size() > 0)
                        meth.arguments.push_back(value::xmlToValue(arr1[0]));
                }
            }
        }
    }

    return meth;
}

//-------------------------------------------------------------------

dodo::data::format::xml::node
method::methodToXml(const rpc::method &data)
{
    dodoArray<dodo::data::format::xml::node> nodeArr;

    dodo::data::format::xml::node meth;
    meth.name = "methodCall";

    dodo::data::format::xml::node methodName;
    methodName.name = "methodName";
    methodName.nodeValue = data.name;

    nodeArr.assign(1, methodName);
    meth.nodeChildren.insert(std::make_pair(methodName.name, nodeArr));

    dodoArray<rpc::value>::const_iterator i = data.arguments.begin(), j = data.arguments.end();
    if (i != j) {
        dodo::data::format::xml::node params;
        params.name = "params";

        dodo::data::format::xml::node param;
        param.name = "param";

        dodoArray<dodo::data::format::xml::node> subNodeArr;

        for (; i != j; ++i) {
            param.nodeChildren.clear();

            nodeArr.assign(1, value::valueToXml(*i));
            param.nodeChildren.insert(std::make_pair("value", nodeArr));

            subNodeArr.push_back(param);
        }
        params.nodeChildren.insert(std::make_pair("param", subNodeArr));

        nodeArr.assign(1, params);
        meth.nodeChildren.insert(std::make_pair(params.name, nodeArr));
    }

    return meth;
}

//-------------------------------------------------------------------
