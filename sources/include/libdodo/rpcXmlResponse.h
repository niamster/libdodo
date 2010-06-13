/***************************************************************************
 *            rpcXmlResponse.h
 *
 *  Wed Apr 09 2008
 *  Copyright  2008  Dmytro Milinevskyy
 *  milinevskyy@gmail.com
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

#ifndef _RPCXMLRESPONSE_H_
#define _RPCXMLRESPONSE_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>

namespace dodo {
    namespace data {
        namespace format {
            namespace xml {
                class node;
            };
        };
    };

    namespace rpc {
        class response;

        namespace xml {
            /**
             * @class response
             * @brief defines RPC response in XML representation
             */
            class response {
              public:

                /**
                 * @return response parsed from XML node
                 * @param node defines XML node
                 */
                static rpc::response xmlToResponse(dodo::data::format::xml::node &node);

                /**
                 * @return XML node parsed from response
                 * @param data defines response structure
                 */
                static dodo::data::format::xml::node responseToXml(const rpc::response &data);
            };
        };
    };
};
#endif
