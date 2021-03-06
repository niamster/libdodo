/***************************************************************************
 *            rpcClient.h
 *
 *  Sat Mar 22 2008
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

#ifndef _RPCCLIENT_H_
#define _RPCCLIENT_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>

namespace dodo {
    namespace io {
        class channel;
    };

    namespace rpc {
        class response;
        class method;

        /**
         * @class client
         * @brief defines client-side RPC instrument
         */
        class client {
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
             * @return rpc response result
             * @param method defines rpc method call
             */
            virtual response call(const method &method);

          protected:

            /**
             * process RPC call
             * @param method defines RPC method representation
             */
            virtual void processCallRequest(const method &method) = 0;

            /**
             * process RPC call
             * @return RPC response represantation
             */
            virtual response processCallResult() = 0;

            const io::channel &io; ///< I/O handler
        };
    };
};
#endif
