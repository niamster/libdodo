/***************************************************************************
 *            rpcXmlServer.cc
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

#include <libdodo/rpcXmlServer.h>

#include <libdodo/types.h>
#include <libdodo/ioChannel.h>
#include <libdodo/rpcServer.h>
#include <libdodo/rpcXmlMethod.h>
#include <libdodo/rpcXmlResponse.h>

using namespace dodo::rpc::xml;

__additionalData__::__additionalData__(dodoString &encoding) : encoding(encoding)
{
}

//-------------------------------------------------------------------

server::server(io::channel &io) : rpc::server(io),
								  rpEncoding("UTF-8")
{
}

//-------------------------------------------------------------------

server::~server()
{
}

//-------------------------------------------------------------------

void
server::setResponseEncoding(const dodoString &a_encoding)
{
	rpEncoding = a_encoding;
}

//-------------------------------------------------------------------

dodo::rpc::method
server::processCall(const dodoString &data)
{
	dodo::data::format::xml::processor xmlValue;

	rqEncoding = xmlValue.getBufferInfo(data).encoding;

	dodo::data::format::xml::__nodeDef__ xmlMethodCall;
	xmlMethodCall.name = "methodCall";
	xmlMethodCall.allChildren = true;

	dodo::data::format::xml::node node = xmlValue.processString(xmlMethodCall, data);

	return method::xmlToMethod(node);
}

//-------------------------------------------------------------------

dodoString
server::processCallResult(const rpc::response &resp)
{
	dodo::data::format::xml::processor xmlValue;

	return xmlValue.make(response::responseToXml(resp), rpEncoding);
}

//-------------------------------------------------------------------

void
server::serve()
{
	try
	{
		rpc::method meth = processCall(io.readStream());

		dodoString encoding = rpEncoding;

		__additionalData__ idata(rqEncoding);

		__additionalData__ odata(rpEncoding);

		dodoMap<dodoString, handler, dodoMapStringCompare>::iterator handler = handlers.find(meth.name);

		if (handler == handlers.end())
		{
			io.writeStream(processCallResult(defaultHandler(meth.name, meth.arguments, &idata, &odata)));
		}
		else
		{
			io.writeStream(processCallResult(handler->second(meth.name, meth.arguments, &idata, &odata)));
		}

		rpEncoding = encoding;
	}
	catch (exception::basic &ex)
	{
		rpc::response response;
		response.fault(ex.baseErrstr);

		io.writeStream(processCallResult(response));
	}
	catch (...)
	{
		rpc::response response;
		response.fault(dodoString("An unknown error."));

		io.writeStream(processCallResult(response));
	}
}

//-------------------------------------------------------------------

