/***************************************************************************
 *            cgiFastExchange.h
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

#ifndef _CGIFASTEXCHANGE_H_
#define _CGIFASTEXCHANGE_H_ 1

#include <libdodo/directives.h>

#ifdef FASTCGI_EXT
#include <libdodo/types.h>
#include <libdodo/cgiExchange.h>

namespace dodo {
    namespace cgi {
        namespace fast {
            struct __request__;

            /**
             * @class exchange
             * @brief provides interface to fast CGI I/O functionality
             */
            class exchange : virtual public cgi::exchange {
              private:

                /**
                 * copy constructor
                 * @note to prevent copying
                 */
                exchange(exchange &);

              public:

                /**
                 * constructor
                 * @param request defines CGI request descriptor
                 * @param protection defines type of IO protection, @see channel::io::channel::protectionEnum
                 */
                exchange(const __request__ &request,
                         short             protection = io::channel::PROTECTION_PROCESS);

                /**
                 * destructor
                 */
                virtual ~exchange();

                /**
                 * flush output
                 */
                virtual void flush() const;

                /**
                 * @return environment variable
                 * @param var defines name of environment variable
                 */
                virtual char *getenv(const char *var);

              protected:

                /**
                 * @return descriptor of input stream
                 */
                virtual int inDescriptor() const;

                /**
                 * @return descriptor of output stream
                 */
                virtual int outDescriptor() const;

                /**
                 * @return amount in bytes of successfully read data
                 * @param data defines buffer that will be filled
                 * @note not more then bs(including null)
                 */
                virtual unsigned long _read(char * const data) const;

                /**
                 * read null- or newline- terminated string
                 * @return amount in bytes of successfully read data
                 * @param data defines buffer that will be filled
                 * @note not more then bs(including null)
                 */
                virtual unsigned long _readString(char * const data) const;

                /**
                 * @return amount in bytes of successfully written data
                 * @param data defines data that will be written
                 */
                virtual unsigned long _write(const char * const data) const;

                /**
                 * write null- terminated string
                 * @return amount in bytes of successfully written data
                 * @param data defines data that will be written
                 * @note puts extra null in the end of the string
                 */
                virtual unsigned long _writeString(const char * const data) const;

              private:

                __request__ *request; ///< fast CGI descriptor
            };
        };
    };
};
#endif
#endif
