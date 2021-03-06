/***************************************************************************
 *            ioNetworkSslExchange.h
 *
 *  Tue Jun 10 2008
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

#ifndef _IONETWORKSSLEXCHANGE_H_
#define _IONETWORKSSLEXCHANGE_H_ 1

#include <libdodo/directives.h>

#ifdef OPENSSL_EXT
#include <libdodo/ioNetworkExchange.h>
#include <libdodo/types.h>

namespace dodo {
    namespace io {
        namespace network {
            namespace http {
                class client;
            };

            namespace ssl {
                struct __connection__;

                /**
                 * @class exchange
                 * @brief provides SSL communication interface[send/receive data]
                 */
                class exchange : public network::exchange {
                    friend class server;
                    friend class client;
                    friend class network::http::client;

                  private:

                    /**
                     * copy constructor
                     * @note to prevent copying
                     */
                    exchange(exchange &);

                  public:

                    /**
                     * @class __init__
                     * @brief holds info that passes to accept call, and then inits exchange;
                     */
                    class __init__ : public network::exchange::__init__ {
                        friend class exchange;
                        friend class client;
                        friend class server;

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

                        io::ssl::__connection__ *handle;         ///< SSL connection handle
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

                    io::ssl::__connection__ *handle; ///< SSL connection handle

                    /**
                     * close socket connection
                     * @param socket defines socket descriptor
                     * @param handle defines SSL handle
                     */
                    void _close(int                     socket,
                                io::ssl::__connection__ *handle);

                    /**
                     * init current instance
                     * @param socket defines socket
                     * @param handle defines SSL handle
                     * @param blocked defines the connection block status
                     * @param blockInherited defines block flag inheritance
                     */
                    void init(int                     socket,
                              io::ssl::__connection__ *handle,
                              bool                    blocked,
                              bool                    blockInherited);

                    /**
                     * @return amount in bytes of successfully read data
                     * @param data defines buffer that will be filled
                     * @note not more than io::channel::bs(including null)
                     */
                    virtual unsigned long _read(char * const data) const;

                    /**
                     * read null- or newline- terminated string
                     * @return amount in bytes of successfully read data
                     * @param data defines buffer that will be filled
                     * @note not more than io::channel::bs(including null)
                     */
                    virtual unsigned long _readString(char * const data) const;

                    /**
                     * @return amount in bytes of successfully written data
                     * @param data defines data that will be written
                     */
                    virtual unsigned long _write(const char * const data) const;
                };
            };
        };
    };
};
#endif
#endif
