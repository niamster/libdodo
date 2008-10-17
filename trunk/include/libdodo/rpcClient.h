/***************************************************************************
 *            rpcClient.h
 *
 *  Sat Mar 22 00:06:55 2008
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

#ifndef _RPCCLIENT_H_
#define _RPCCLIENT_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/toolsString.h>
#include <libdodo/rpcMethod.h>
#include <libdodo/rpcResponse.h>

namespace dodo
{
	namespace rpc
	{
		/**
		 * @class client defines client-side RPC instrument
		 */
		class client
		{
			public:

				/**
				 * constructor
				 */
				client();

				/**
				 * destructor
				 */
				virtual ~client();

				/**
				 * @return rpc response result
				 * @param method defines rpc method call
				 */
				virtual response call(const method &method);

			protected:

				/**
				 * process RPC call
				 * @return RPC method
				 * @param method defines RPC method representation
				 */
				virtual dodoString processCall(const method &method) = 0;

				/**
				 * process RPC call
				 * @return RPC response represantation
				 * @param data defines buffer that contains RPC response
				 */
				virtual response processCallResult(const dodoString &data) = 0;

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
		};
	};
};

#endif
