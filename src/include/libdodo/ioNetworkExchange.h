/***************************************************************************
 *            ioNetworkExchange.h
 *
 *  Thu Oct 04 2005
 *  Copyright  2005  Ni@m
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

#ifndef _IONETWORKEXCHANGE_H_
#define _IONETWORKEXCHANGE_H_ 1

#include <libdodo/directives.h>

#include <libdodo/ioNetworkConnection.h>
#include <libdodo/ioChannel.h>
#include <libdodo/ioStreamChannel.h>
#include <libdodo/xexec.h>

namespace dodo {
    namespace io {
        namespace network {
            namespace ssl {
                class server;
            };

            /**
             * @class exchange
             * @brief provides communication interface[send/receive data]
             */
            class exchange : public connection,
                             virtual public stream::channel {
                friend class server;
                friend class client;
                friend class http::client;

              private:

                /**
                 * copy constructor
                 * @note to prevent copying
                 */
                exchange(exchange &);

              public:

                /**
                 * @enum operationEnum defines type of operation for xexec
                 */
                enum operationEnum {
                    OPERATION_CLOSE = 128,
                };

                /**
                 * @class __init__
                 * @brief holds info that is passed to accept call, and then inits exchange
                 */
                class __init__ {
                    friend class exchange;
                    friend class client;
                    friend class server;
                    friend class ssl::server;

                  public:

                    /**
                     * constructor
                     */
                    __init__();

                    /**
                     * copy constructor
                     * @note if you want to copy it, the object, from what has been copied is not more able to init new session: you have to reinit it with ::accept method
                     */
                    __init__(__init__ &);

                    /**
                     * destructor
                     */
                    virtual ~__init__();

                  protected:

                    int socket;             ///< socket

                    bool blocked;           ///< true if blocked
                    bool blockInherited;    ///< true if block flag is inherited
                };

                /**
                 * constructor
                 * @param protection defines type of IO protection, @see io::channel::protectionEnum
                 */
                exchange(short protection = channel::PROTECTION_PROCESS);

                /**
                 * constructor
                 * @param init is initial data[got from the ::accept method]
                 * @param protection defines type of IO protection, @see io::channel::protectionEnum
                 * @note the object that has inited the object of current instance can be used for another connections
                 */
                exchange(__init__ &init,
                         short    protection = channel::PROTECTION_PROCESS);

                /**
                 * destructor
                 */
                virtual ~exchange();

                /**
                 * @return true if connection is alive
                 */
                virtual bool isAlive();

                /**
                 * close connection
                 */
                virtual void close();

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
                 * init current instance
                 * @param socket defines socket
                 * @param blocked defines the connection block status
                 * @param blockInherited defines block flag inheritance
                 */
                void init(int  socket,
                          bool blocked,
                          bool blockInherited);

                /**
                 * flush output
                 * @note does nothing for network connections
                 */
                virtual void flush() const;

                /**
                 * @return successfully read bytes
                 * @param data defines buffer that will be filled
                 * @note not more then bs(including null)
                 */
                virtual unsigned long _read(char * const data) const;

                /**
                 * read null- or newline- terminated string
                 * @return successfully read bytes
                 * @param data defines buffer that will be filled
                 * @note not more then bs(including null)
                 */
                virtual unsigned long _readString(char * const data) const;

                /**
                 * @return successfully written bytes
                 * @param data defines data that will be written
                 */
                virtual unsigned long _write(const char * const data) const;

                /**
                 * write null- terminated string
                 * @return successfully written bytes
                 * @param data defines data that will be written
                 * @note puts extra null in the end of the string
                 */
                virtual unsigned long _writeString(const char * const data) const;
            };
        };
    };
};
#endif
