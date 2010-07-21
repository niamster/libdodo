/***************************************************************************
 *            rpcServer.h
 *
 *  Sat Mar 29 2008
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

#ifndef _RPCSERVER_H_
#define _RPCSERVER_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/rpcValue.h>

namespace dodo {
    namespace io {
        class channel;
    };

    namespace rpc {
        class response;
        class method;

        /**
         * @typedef handler
         * @brief defines type of rpc method handler
         * @param method defines method name
         * @param arguments defines arguments of the method
         * @param idata defines additional incoming data
         * @param odata defines additional outgoing data
         */
        typedef response (*handler)(const dodo::string &method, const dodoArray<value> &arguments, const void *idata, void *odata);

        /**
         * @class server
         * @brief defines server-side RPC instrument
         */
        class server {
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
             * set handler for method
             * @param method defines name of rpc method
             * @param handler defines function that will process rpc call
             */
            void setHandler(const dodo::string &method,
                            handler          handler);

            /**
             * remove handler for method
             * @param method defines name of rpc method
             */
            void removeHandler(const dodo::string &method);

            /**
             * set function that will process unregistered calls
             * @param handler defines function that will process rpc call
             */
            void setDefault(handler handler);

            /**
             * serve rpc call
             * @note processes only one call
             * should be called again to process next
             * values of idata and odata for handler are NULL
             */
            virtual void serve();

          protected:

            /**
             * process RPC call
             * @return RPC method represantation
             */
            virtual method processCallRequest() = 0;

            /**
             * process RPC call
             * @param response defines RPC response representation
             */
            virtual void processCallResult(const response &response) = 0;

            /**
             * default rpc handler
             * @param method defines function that will process rpc call
             * @param arguments defines method arguments
             * @param idata defines additional incoming data
             * @param odata defines additional outgoing data
             */
            static response rpcDefaultHandler(const dodo::string       &method,
                                              const dodoArray<value> &arguments,
                                              const void             *idata,
                                              void                   *odata);

            dodoMap<dodo::string, handler, dodoMapStringCompare> handlers;    ///< method handlers
            handler defaultHandler;                                         ///< default handler

            io::channel &io;                                                ///< I/O handler
        };
    };
};
#endif

