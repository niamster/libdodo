/***************************************************************************
 *            rpcServer.cc
 *
 *  Sat Mar 29 2008
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

#include <libdodo/rpcServer.h>
#include <libdodo/ioChannel.h>
#include <libdodo/types.h>
#include <libdodo/rpcValue.h>
#include <libdodo/rpcResponse.h>
#include <libdodo/rpcMethod.h>
#include <libdodo/exceptionBasic.h>

using namespace dodo::rpc;

server::server(io::channel &io) : defaultHandler(&rpcDefaultHandler),
                                  io(io)
{
}

//-------------------------------------------------------------------

server::~server()
{
}

//-------------------------------------------------------------------

response
server::rpcDefaultHandler(const dodo::string &,
                          const dodoArray<value> &,
                          const void *,
                          void *)
{
    response response;
    response.fault(dodo::string("rpcDefaultHandler"));

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
server::setHandler(const dodo::string &method,
                   handler          handler)
{
    handlers.insert(std::make_pair(method, handler));
}

//-------------------------------------------------------------------

void
server::removeHandler(const dodo::string &method)
{
    handlers.erase(method);
}

//-------------------------------------------------------------------

void
server::serve()
{
    try {
        method meth = processCallRequest();

        dodoMap<dodo::string, handler, dodoMapStringCompare>::iterator handler = handlers.find(meth.name);

        if (handler == handlers.end())
            processCallResult(defaultHandler(meth.name, meth.arguments, NULL, NULL));
        else
            processCallResult(handler->second(meth.name, meth.arguments, NULL, NULL));
    } catch (exception::basic &ex) {
        response response;
        response.fault(ex.errStr);

        processCallResult(response);
    } catch (...) {
        response response;
        response.fault(dodo::string("An unknown error."));

        processCallResult(response);
    }
}

//-------------------------------------------------------------------
