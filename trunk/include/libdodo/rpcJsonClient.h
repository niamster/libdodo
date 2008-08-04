/***************************************************************************
 *            rpcJsonClient.h
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

#ifndef _RPCJSONCLIENT_H_
#define _RPCJSONCLIENT_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/toolsString.h>
#include <libdodo/rpcClient.h>
#include <libdodo/rpcJsonMethod.h>
#include <libdodo/rpcJsonResponse.h>

namespace dodo
{
	namespace rpc
	{
		namespace json
		{
			/**
			 * @class client defines client-side JSON-RPC instrument
			 */
			class client : virtual public rpc::client
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
					 * set version of JSON-RPC request
					 * @param version defines version of JSON-RPC request
					 */
					virtual void setRequestVersion(const dodoString &version);

					/**
					 * @return version of JSON-RPC response
					 */
					virtual dodoString getResponseVersion();

					/**
					 * @return ID of JSON-RPC response
					 */
					virtual long getResponseId();

				protected:

					/**
					 * process RPC call
					 * @return RPC method
					 * @param meth defines RPC method representation
					 */
					virtual dodoString processCall(const rpc::method &meth);

					/**
					 * process RPC call
					 * @return RPC response represantation
					 * @param data defines buffer that contains RPC response
					 */
					virtual rpc::response processCallResult(const dodoString &data);

					dodoString rqVersion; ///< request version['1.1' by default]
					dodoString rpVersion; ///< response version

					long rqId; ///< request ID
					long rpId; ///< response ID
			};
		};
	};
};

#endif
