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

#include <libdodo/directives.h>

#include <libdodo/rpcJsonServer.h>
#include <libdodo/types.h>
#include <libdodo/dataFormatJsonProcessor.h>
#include <libdodo/dataFormatJsonNode.h>
#include <libdodo/ioChannel.h>
#include <libdodo/rpcServer.h>
#include <libdodo/rpcJsonMethod.h>
#include <libdodo/rpcMethod.h>
#include <libdodo/rpcJsonResponse.h>
#include <libdodo/rpcResponse.h>
#include <libdodo/exceptionBasic.h>

using namespace dodo::rpc::json;

__additionalData__::__additionalData__(dodoString &version,
									   long       &id) : version(version),
														 id(id)
{
}

//-------------------------------------------------------------------

server::server(io::channel &io) : rpc::server(io),
								  rpVersion("1.1"),
								  rpId(0),
								  rqId(0)
{
}

//-------------------------------------------------------------------

server::~server()
{
}

//-------------------------------------------------------------------

dodo::rpc::method
server::processCallRequest()
{
	dodo::data::format::json::processor jsonValue;

	dodo::data::format::json::node node = jsonValue.process(io);

	return method::jsonToMethod(node, rqVersion, rqId);
}

//-------------------------------------------------------------------

void
server::processCallResult(const rpc::response &resp)
{
	dodo::data::format::json::processor jsonValue;

	jsonValue.make(response::responseToJson(resp, rpVersion, rpId), io);

	io.flush();
}

//-------------------------------------------------------------------

void
server::setResponseVersion(const dodoString &version)
{
	rpVersion = version;
}

//-------------------------------------------------------------------

void
server::serve()
{
	try {
		rpc::method meth = processCallRequest();

		dodoString version = rqVersion;

		__additionalData__ idata(rqVersion, rqId);

		rpId = rqId;
		__additionalData__ odata(rpVersion, rpId);

		dodoMap<dodoString, handler, dodoMapStringCompare>::iterator handler = handlers.find(meth.name);

		if (handler == handlers.end())
			processCallResult(defaultHandler(meth.name, meth.arguments, &idata, &odata));
		else
			processCallResult(handler->second(meth.name, meth.arguments, &idata, &odata));

		rpVersion = version;
	} catch (exception::basic &ex) {
		rpc::response response;
		response.fault(ex.baseErrstr);

		rpId = rqId;

		processCallResult(response);
	} catch (...) {
		rpc::response response;
		response.fault(dodoString("An unknown error."));

		rpId = rqId;

		processCallResult(response);
	}
}

//-------------------------------------------------------------------

