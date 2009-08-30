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

#include <libdodo/directives.h>

#include <libdodo/rpcXmlResponse.h>
#include <libdodo/types.h>
#include <libdodo/dataFormatXmlNode.h>
#include <libdodo/toolsString.h>
#include <libdodo/rpcResponse.h>
#include <libdodo/rpcValue.h>
#include <libdodo/rpcXmlValue.h>

using namespace dodo::rpc::xml;

dodo::rpc::response
response::xmlToResponse(dodo::data::format::xml::node &node)
{
	dodoMap<dodoString, dodoArray<dodo::data::format::xml::node>, dodoMapStringCompare>::iterator i = node.nodeChildren.begin();
	if (i == node.nodeChildren.end())
		return rpc::response();

	rpc::response resp;

	if (tools::string::iequal(i->first, "fault")) {
		resp.succ = false;

		dodoArray<dodo::data::format::xml::node> &arr0 = i->second;
		if (arr0.size() > 0) {
			dodoArray<dodo::data::format::xml::node> &arr1 = arr0[0].nodeChildren["nodeValue"];
			if (arr1.size() > 0)
				resp.rValues.assign(1, value::xmlToValue(arr1[0]));
		}
	} else {
		if (tools::string::iequal(i->first, "params")) {
			resp.succ = true;

			dodoArray<dodo::data::format::xml::node> &arr0 = i->second;
			if (arr0.size() == 0)
				return resp;

			dodoArray<dodo::data::format::xml::node> &nodeArray = arr0[0].nodeChildren["param"];

			dodoArray<dodo::data::format::xml::node>::iterator o = nodeArray.begin(), p = nodeArray.end();
			for (; o != p; ++o) {
				dodoArray<dodo::data::format::xml::node> &arr1 = o->nodeChildren["nodeValue"];
				if (arr1.size() > 0)
					resp.rValues.push_back(value::xmlToValue(arr1[0]));
			}
		}
	}

	return resp;
}

//-------------------------------------------------------------------

dodo::data::format::xml::node
response::responseToXml(const rpc::response &data)
{
	dodoArray<dodo::data::format::xml::node> nodeArr;

	dodo::data::format::xml::node resp;
	resp.name = "methodResponse";

	if (data.succ) {
		dodo::data::format::xml::node params;
		params.name = "params";

		dodo::data::format::xml::node param;
		param.name = "param";

		dodoArray<dodo::data::format::xml::node> subNodeArr;

		dodoArray<rpc::value>::const_iterator i = data.rValues.begin(), j = data.rValues.end();
		for (; i != j; ++i) {
			param.nodeChildren.clear();

			nodeArr.assign(1, value::valueToXml(*i));
			param.nodeChildren.insert(make_pair("nodeValue", nodeArr));

			subNodeArr.push_back(param);
		}
		params.nodeChildren.insert(make_pair("param", subNodeArr));

		nodeArr.assign(1, params);
		resp.nodeChildren.insert(make_pair(params.name, nodeArr));
	} else {
		dodo::data::format::xml::node fault;
		fault.name = "fault";

		dodoArray<rpc::value>::const_iterator i = data.rValues.begin();
		if (i != data.rValues.end()) {
			nodeArr.assign(1, value::valueToXml(*i));
			fault.nodeChildren.insert(make_pair("nodeValue", nodeArr));
		}

		nodeArr.assign(1, fault);
		resp.nodeChildren.insert(make_pair(fault.name, nodeArr));
	}

	return resp;
}

//-------------------------------------------------------------------
