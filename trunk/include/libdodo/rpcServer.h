/***************************************************************************
 *            rpcServer.h
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

#ifndef _RPCSERVER_H_
#define _RPCSERVER_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/toolsString.h>
#include <libdodo/rpcValue.h>
#include <libdodo/rpcResponse.h>
#include <libdodo/rpcMethod.h>

namespace dodo
{	
	namespace rpc
	{
		/**
		 * @typedef handler defines type of rpc method handler
		 */
		typedef response (*handler)(const dodoString &, const dodoArray<value> &);
		
		/**
		 * @class server defines server-side RPC instrument
		 */
		class server
		{
			public:
	
				/**
				 * constructor
				 */
				server();
	
				/**
				 * destructor
				 */
				virtual ~server();
			
				/**
				 * set handler for method
				 * @param method defines name of rpc method
				 * @param handler defines function that will process rpc call
				 */
				virtual void setHandler(const dodoString &method, handler handler);
			
				/**
				 * remove handler for method
				 * @param method defines name of rpc method
				 */
				virtual void removeHandler(const dodoString &method);
			
				/**
				 * set function that will process unregistered calls
				 * @param handler defines function that will process rpc call
				 */
				virtual void setDefault(handler handler);
				
				/**
				 * serve rpc call
				 */
				virtual void serve();
			
			protected:
				
				/**
				 * process RPC call
				 * @return RPC method represantation
				 * @param data defines buffer that contains RPC request
				 */
				virtual method processRpcCall(const dodoString &data) = 0;
				
				/**
				 * process RPC call
				 * @return RPC response
				 * @param response defines RPC response representation
				 */
				virtual dodoString processRpcCallResult(const response &response) = 0;
				
				/**
				 * default rpc handler
				 * @param method defines function that will process rpc call
				 * @param arguments defines method arguments
				 */
				static response rpcDefaultHandler(const dodoString &method, const dodoArray<value> &arguments);
			
				/**
				 * send request
				 * @param method defines rpc method call
				 */
				virtual void sendTextRequest(const dodoString &method) = 0;
				
				/**
				 * get response
				 * @return rpc response result 
				 */
				virtual dodoString receiveTextResponse() = 0;
				
				dodoMap<dodoString, handler, dodoMapStringCompare> handlers;///< method handlers
				handler defaultHandler;///< default handler
		};
	};
};

#endif
