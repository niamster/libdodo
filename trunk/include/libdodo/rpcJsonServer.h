/***************************************************************************
 *            rpcJsonServer.h
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

#ifndef _RPCJSONSERVER_H_
#define _RPCJSONSERVER_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/rpcServer.h>

namespace dodo {
	namespace io {
		class channel;
	};

	namespace rpc {
		class method;
		class response;

		namespace json {
			/**
			 * @struct __additional__
			 * @brief defines response/request additional data
			 */
			struct __additional__ {
				/**
				 * constructor
				 * @param version defines response/request version
				 * @param id defines response/request ID
				 */
				__additional__(dodoString &version, long &id);

				dodoString &version;    ///< response/request version
				long       &id;         ///< response/request ID
			};

			/**
			 * @class server
			 * @brief defines server-side JSON-RPC instrument
			 */
			class server : virtual public rpc::server {
			  public:

				/**
				 * constructor
				 * @param io defines input/output sources
				 */
				server(io::channel &io);

				/**
				 * destructor
				 */
				virtual ~server();

				/**
				 * set version of JSON-RPC response
				 * @param version defines version of JSON-RPC response
				 */
				virtual void setResponseVersion(const dodoString &version);

				/**
				 * serve rpc call
				 * @note processes only one call
				 * should be called again to process next
				 * default values of odata for handler are set by setResponseVersion method and request ID
				 */
				virtual void serve();

			  protected:

				/**
				 * process RPC call
				 * @return RPC method represantation
				 */
				virtual rpc::method processCallRequest();

				/**
				 * process RPC call
				 * @return RPC response
				 * @param response defines RPC response representation
				 */
				virtual void processCallResult(const rpc::response &response);

				dodoString rqVersion;   ///< request version
				dodoString rpVersion;   ///< response version['1.1' by default]

				long rqId;              ///< request ID
				long rpId;              ///< response ID
			};
		};
	};
};
#endif

