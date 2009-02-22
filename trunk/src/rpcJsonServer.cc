/***************************************************************************
 *            rpcJsonServer.cc
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

#include <libdodo/rpcJsonServer.h>

using namespace dodo::rpc::json;

__additionalData::__additionalData(dodoString &version,
								   long       &id) : version(version),
													 id(id)
{
}

//-------------------------------------------------------------------

server::server() : rpVersion("1.1"),
				   rpId(0),
				   rqId(0)
{
}

//-------------------------------------------------------------------

server::~server()
{
}

//-------------------------------------------------------------------

dodo::rpc::method server::processCall(const dodoString &data)
{
	dodo::data::format::json::processor jsonValue;

	dodo::data::format::json::node node = jsonValue.processString(data);

	return method::jsonToMethod(node, rqVersion, rqId);
}

//-------------------------------------------------------------------

dodoString server::processCallResult(const rpc::response &resp)
{
	dodo::data::format::json::processor jsonValue;

	return jsonValue.make(response::responseToJson(resp, rpVersion, rpId));
}

//-------------------------------------------------------------------

void server::setResponseVersion(const dodoString &version)
{
	rpVersion = version;
}

//-------------------------------------------------------------------

void server::serve()
{
	try
	{
		rpc::method meth = processCall(receiveTextResponse());

		dodoString version = rqVersion;

		__additionalData idata(rqVersion, rqId);

		rpId = rqId;
		__additionalData odata(rpVersion, rpId);

		dodoMap<dodoString, handler, dodoMapStringCompare>::iterator handler = handlers.find(meth.name);

		if (handler == handlers.end())
		{
			sendTextRequest(processCallResult(defaultHandler(meth.name, meth.arguments, &idata, &odata)));
		}
		else
		{
			sendTextRequest(processCallResult(handler->second(meth.name, meth.arguments, &idata, &odata)));
		}

		rpVersion = version;
	}
	catch (exception::basic &ex)
	{
		rpc::response response;
		response.fault(ex.baseErrstr);

		rpId = rqId;

		sendTextRequest(processCallResult(response));
	}
	catch (...)
	{
		rpc::response response;
		response.fault(dodoString("An unknown error."));

		rpId = rqId;

		sendTextRequest(processCallResult(response));
	}
}

//-------------------------------------------------------------------

