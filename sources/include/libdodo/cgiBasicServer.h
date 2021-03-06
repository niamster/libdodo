/***************************************************************************
 *            cgiBasicServer.h
 *
 *  Sat Aug  5 2006
 *  Copyright  2006  Dmytro Milinevskyy
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

#ifndef _CGIBASICSERVER_H_
#define _CGIBASICSERVER_H_ 1

#include <libdodo/directives.h>

#include <libdodo/cgiServer.h>

namespace dodo {
    namespace cgi {
        namespace basic {
            /**
             * @class server
             * @brief provides basic CGI server functionality
             */
            class server : public cgi::server {
              private:

                /**
                 * copy constructor
                 * @note to prevent copying
                 */
                server(server &);

              public:

                /**
                 * constructor
                 */
                server();

                /**
                 * destructor
                 */
                virtual ~server();

                /**
                 * listen for incoming requests
                 * @param handler defines request handler
                 */
                virtual void serve(handler handler);
            };
        };
    };
};
#endif
