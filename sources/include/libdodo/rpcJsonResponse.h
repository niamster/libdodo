/***************************************************************************
 *            rpcJsonResponse.h
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

#ifndef _RPCJSONRESPONSE_H_
#define _RPCJSONRESPONSE_H_ 1

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
        class response;
        namespace json {
            /**
             * @class response
             * @brief defines RPC response in JSON representation
             */
            class response {
              public:

                /**
                 * @return response parsed from JSON node
                 * @param node defines JSON node
                 * @param version defines version of JSON-RPC response
                 * @param id defines ID of JSON-RPC response
                 */
                static rpc::response jsonToResponse(dodo::data::format::json::node &node,
                                                    dodo::string                     &version,
                                                    long                           &id);

                /**
                 * @return JSON node parsed from response
                 * @param data defines response structure
                 * @param version defines version of JSON-RPC request
                 * @param id defines ID of JSON-RPC request
                 */
                static dodo::data::format::json::node responseToJson(const rpc::response &data,
                                                                     const dodo::string    &version,
                                                                     long                id);
            };
        };
    };
};
#endif
