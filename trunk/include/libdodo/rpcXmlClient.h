/***************************************************************************
 *            rpcXmlClient.h
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _RPCXMLCLIENT_H_
#define _RPCXMLCLIENT_H_ 1

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

		namespace xml {
			/**
			 * @class client
			 * @brief defines client-side XML-RPC instrument
			 */
			class client : virtual public rpc::client {
			  public:

				/**
				 * constructor
				 */
				client(const io::channel &io);

				/**
				 * destructor
				 */
				virtual ~client();

				/**
				 * @param encoding defines request encoding
				 */
				void setRequestEncoding(const dodoString &encoding);

				/**
				 * @return response encoding
				 */
				dodoString getResponseEncoding();

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

				dodoString rpEncoding;  ///< encoding of the messages['UTF-8' by default]
				dodoString rqEncoding;  ///< encoding of the messages['UTF-8' by default]
			};
		};
	};
};

#endif
