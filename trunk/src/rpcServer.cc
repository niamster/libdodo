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

#include <libdodo/rpcServer.h>

using namespace dodo;

rpcServer::rpcServer() : defaultHandler(&rpcDefaultHandler)
{
	
}

//-------------------------------------------------------------------

rpcServer::~rpcServer()
{
	
}

//-------------------------------------------------------------------

rpcResponse 
rpcServer::rpcDefaultHandler(const dodoString &method, 
		const dodoMap<dodoString, rpcValue, dodoMapStringCompare> &arguments)
{
	return rpcResponse();
}

//-------------------------------------------------------------------

void 
rpcServer::setDefault(rpcHandler handler)
{
	defaultHandler = handler;
}

//-------------------------------------------------------------------

void 
rpcServer::setHandler(const dodoString &method, 
					rpcHandler handler)
{
	handlers.insert(make_pair(method, handler));
}

//-------------------------------------------------------------------

void 
rpcServer::removeHandler(const dodoString &method)
{
	handlers.erase(method);
}

//-------------------------------------------------------------------

void 
rpcServer::serve()
{
	rpcMethod method = processRPCCall(receiveTextResponse());
	
	dodoMap<dodoString, rpcHandler, dodoMapStringCompare>::iterator end = handlers.end();
	
	dodoMap<dodoString, rpcHandler, dodoMapStringCompare>::iterator handler = handlers.find(method.name);
	if (handler == end)
		sendTextRequest(processRPCCallResult(defaultHandler(method.name, method.arguments)));
	else
		sendTextRequest(processRPCCallResult(handler->second(method.name, method.arguments)));
}

//-------------------------------------------------------------------
