/***************************************************************************
 *            rpcXmlServer.h
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

#ifndef _RPCXMLSERVER_H_
#define _RPCXMLSERVER_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/toolsString.h>
#include <libdodo/rpcServer.h>
#include <libdodo/rpcXmlMethod.h>
#include <libdodo/rpcXmlResponse.h>

namespace dodo
{
	
	/**
	 * @class rpcXmlServer defines server-side XML-RPC instrument
	 */
	class rpcXmlServer : public rpcServer
	{
		public:

			/**
			 * constructor
			 */
			rpcXmlServer();

			/**
			 * destructor
			 */
			virtual ~rpcXmlServer();
		
		protected:
			
			/**
			 * process RPC call
			 * @return RPC method represantation
			 * @param data defines buffer that contains RPC request
			 */
			virtual rpcMethod processRPCCall(const dodoString &data);
			
			/**
			 * process RPC call
			 * @return RPC response
			 * @param response defines RPC response representation
			 */
			virtual dodoString processRPCCallResult(const rpcResponse &response);
	};
};

#endif
