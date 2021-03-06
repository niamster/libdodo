/***************************************************************************
 *            rpcXmlServer.h
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

#ifndef _RPCXMLSERVER_H_
#define _RPCXMLSERVER_H_ 1

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

        namespace xml {
            /**
             * @struct __additional__
             * @brief defines response/request additional data
             */
            struct __additional__ {
                /**
                 * constructor
                 * @param encoding defines response/request encoding
                 */
                __additional__(dodo::string &encoding);

                dodo::string &encoding; ///< response/request encoding
            };

            /**
             * @class server
             * @brief defines server-side XML-RPC instrument
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
                 * @param encoding defines response encoding
                 */
                void setResponseEncoding(const dodo::string &encoding);

                /**
                 * serve rpc call
                 * @note processes only one call
                 * should be called again to process next
                 * default values of odata for handler are set by setResponseEncoding method
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
                 * @param response defines RPC response representation
                 */
                virtual void processCallResult(const rpc::response &response);

                dodo::string rpEncoding;  ///< encoding of the messages['UTF-8' by default]
                dodo::string rqEncoding;  ///< encoding of the messages['UTF-8' by default]
            };
        };
    };
};
#endif
