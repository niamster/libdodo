/***************************************************************************
 *            rpcJsonMethod.h
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

#ifndef _RPCJSONMETHOD_H_
#define _RPCJSONMETHOD_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/toolsString.h>
#include <libdodo/rpcMethod.h>
#include <libdodo/rpcJsonValue.h>
#include <libdodo/jsonProcessor.h>

namespace dodo
{
	namespace rpc
	{
		namespace json
		{
			/**
			 * @class method defines RPC method in JSON representation
			 */
			class method
			{
				friend class client;

				public:

					/**
					 * @return method parsed from JSON
					 * @param data defines JSON string
					 */
					static rpc::method jsonToRpcMethod(const dodoString &data);

					/**
					 * @return JSON parsed from method
					 * @param data defines method structure
					 */
					static dodoString methodToJson(const rpc::method &data);

				protected:

					/**
					 * @return method parsed from JSON node
					 * @param node defines JSON node
					 */
					static rpc::method jsonToRpcMethod(dodo::json::node &node);

					/**
					 * @return JSON node parsed from method
					 * @param data defines method structure
					 */
					static dodo::json::node methodToJsonNode(const rpc::method &data);
			};
		};
	};
};

#endif
