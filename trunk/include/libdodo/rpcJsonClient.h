/***************************************************************************
 *            rpcJsonClient.h
 *
 *  Mon Jul 07 2008
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
#define _RPCJSONCLIENT_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/rpcClient.h>

namespace dodo {
	namespace io {
		class channel;
	};

	namespace rpc {
		class method;
		class response;

		namespace json {
			/**
			 * @class client
			 * @brief defines client-side JSON-RPC instrument
			 */
			class client : virtual public rpc::client {
			  public:

				/**
				 * constructor
				 * @param io defines input/output sources
				 */
				client(const io::channel &io);

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
				virtual dodoString responseVersion();

				/**
				 * @return ID of JSON-RPC response
				 */
				virtual long responseId();

			  protected:

				/**
				 * process RPC call
				 * @param meth defines RPC method representation
				 */
				virtual void processCallRequest(const rpc::method &meth);

				/**
				 * process RPC call
				 * @return RPC response represantation
				 */
				virtual rpc::response processCallResult();

				dodoString rqVersion;   ///< request version['1.1' by default]
				dodoString rpVersion;   ///< response version

				long rqId;              ///< request ID
				long rpId;              ///< response ID
			};
		};
	};
};
#endif
