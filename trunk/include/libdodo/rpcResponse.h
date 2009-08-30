/***************************************************************************
 *            rpcResponse.h
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

#ifndef _RPCRESPONSE_H_
#define _RPCRESPONSE_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/rpcValue.h>

namespace dodo {
	namespace rpc {
		namespace xml {
			class response;
		};

		namespace json {
			class response;
		};

		/**
		 * @class response
		 * @brief defines RPC response reprasentation
		 */
		class response {
			friend class client;
			friend class server;
			friend class xml::response;
			friend class json::response;

		  public:

			/**
			 * constructor
			 */
			response();

			/**
			 * destructor
			 */
			~response();

			/**
			 * @return response value
			 * @param index defines index of returned value
			 */
			rpc::value value(unsigned long index = 0);

			/**
			 * @return response values
			 */
			dodoArray<rpc::value> values();

			/**
			 * @return response values count
			 */
			unsigned long valuesCount();

			/**
			 * @return true if request was successful
			 */
			bool isSuccessful();

			/**
			 * set argument for fault response
			 * @param argument defines response argument
			 */
			void fault(const rpc::value &argument);

			/**
			 * add argument for successful response
			 * @param argument defines response argument
			 */
			void addArgument(const rpc::value &argument);

			/**
			 * @return value for type casting
			 * @param index defines index of returned value
			 */
			rpc::value operator[](unsigned long index);

			/**
			 * clear arguments information
			 */
			void clear();

		  private:

			dodoArray<rpc::value> rValues;    ///< response values
			bool succ;                  ///< true if server returned non-fail response
		};
	};
};
#endif
