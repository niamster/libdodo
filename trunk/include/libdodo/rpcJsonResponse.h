/***************************************************************************
 *            rpcJsonResponse.h
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

#ifndef _RPCJSONRESPONSE_H_
#define _RPCJSONRESPONSE_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/toolsString.h>
#include <libdodo/rpcResponse.h>
#include <libdodo/rpcJsonValue.h>
#include <libdodo/jsonProcessor.h>

namespace dodo
{
	namespace rpc
	{
		namespace json
		{
			/**
			 * @class response defines RPC response in JSON representation
			 */
			class response
			{
				friend class server;

				public:

					/**
					 * @return response parsed from JSON
					 * @param data defines JSON string
					 */
					static rpc::response jsonToRpcResponse(const dodoString &data);

					/**
					 * @return JSON parsed from response
					 * @param data defines response structure
					 */
					static dodoString responseToJson(const rpc::response &data);

				protected:

					/**
					 * @return response parsed from JSON node
					 * @param node defines JSON node
					 */
					static rpc::response jsonToRpcResponse(dodo::json::node &node);

					/**
					 * @return JSON node parsed from response
					 * @param data defines response structure
					 */
					static dodo::json::node responseToJsonNode(const rpc::response &data);
			};
		};
	};
};

#endif
