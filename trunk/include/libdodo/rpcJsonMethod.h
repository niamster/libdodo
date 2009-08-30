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
#define _RPCJSONMETHOD_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>

namespace dodo {
	namespace data {
		namespace format {
			namespace json {
				class node;
			};
		};
	};

	namespace rpc {
		class method;

		namespace json {
			/**
			 * @class method
			 * @brief defines RPC method in JSON representation
			 */
			class method {
			  public:

				/**
				 * @return method parsed from JSON node
				 * @param node defines JSON node
				 * @param version defines version of JSON-RPC response
				 * @param id defines ID of JSON-RPC response
				 */
				static rpc::method jsonToMethod(dodo::data::format::json::node &node,
												dodoString                     &version,
												long                           &id);

				/**
				 * @return JSON node parsed from method
				 * @param data defines method structure
				 * @param version defines version of JSON-RPC request
				 * @param id defines ID of JSON-RPC request
				 */
				static dodo::data::format::json::node methodToJson(const rpc::method &data,
																   const dodoString  &version,
																   long              id);
			};
		};
	};
};
#endif
