/***************************************************************************
 *            rpcServer.cc
 *
 *  Sat Mar 29 00:06:55 2008
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

#include <libdodo/rpcServer.h>

using namespace dodo::rpc;

server::server() : defaultHandler(&rpcDefaultHandler)
{

}

//-------------------------------------------------------------------

server::~server()
{

}

//-------------------------------------------------------------------

response
server::rpcDefaultHandler(const dodoString &method,
						  const dodoArray<value> &arguments)
{
	response response;
	response.fault(dodoString("rpcDefaultHandler"));

	return response;
}

//-------------------------------------------------------------------

void
server::setDefault(handler handler)
{
	defaultHandler = handler;
}

//-------------------------------------------------------------------

void
server::setHandler(const dodoString &method,
				   handler handler)
{
	handlers.insert(make_pair(method, handler));
}

//-------------------------------------------------------------------

void
server::removeHandler(const dodoString &method)
{
	handlers.erase(method);
}

//-------------------------------------------------------------------

void
server::serve()
{
	try
	{
		method method = processCall(receiveTextResponse());

		dodoMap<dodoString, handler, dodoMapStringCompare>::iterator end = handlers.end();

		dodoMap<dodoString, handler, dodoMapStringCompare>::iterator handler = handlers.find(method.name);

		if (handler == end)
			sendTextRequest(processCallResult(defaultHandler(method.name, method.arguments)));
		else
			sendTextRequest(processCallResult(handler->second(method.name, method.arguments)));
	}
	catch (baseEx ex)
	{
		response response;
		response.fault(ex.baseErrstr);

		sendTextRequest(processCallResult(response));
	}
	catch (...)
	{
		response response;
		response.fault(dodoString("An unknown error."));

		sendTextRequest(processCallResult(response));
	}
}

//-------------------------------------------------------------------
