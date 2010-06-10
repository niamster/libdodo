/***************************************************************************
 *            ioNetworkConnection.h
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

#ifndef _IONETWORKCONNECTION_H_
#define _IONETWORKCONNECTION_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>

namespace dodo {
    namespace io {
        namespace network {
            namespace http {
                class client;
            };

            /**
             * @class connection
             * @brief provides option manipulation for network connections
             */
            class connection {
                friend class http::client;

              protected:

                /**
                 * constructor
                 */
                connection();

                /**
                 * destructor
                 */
                virtual ~connection();

              public:

                /**
                 * @struct __peer__
                 * @brief defines peer information
                 */
                struct __peer__ {
                    dodo::string host;                        ///< host of the box
                    int        port;                        ///< port of the box
                };

                /**
                 * @enum transferEnum defines type of socket
                 */
                enum transferEnum {
                    TRANSFER_STREAM,                        ///< Sequenced, reliable, connection-based byte streams
                    TRANSFER_DATAGRAM,                      ///< Connectionless, unreliable datagrams of fixed maximum length
                };

                /**
                 * @enum protocolFamilyEnum defines type of domain of socket
                 */
                enum protocolFamilyEnum {
                    PROTOCOL_FAMILY_IPV4,
                    PROTOCOL_FAMILY_IPV6,
                    PROTOCOL_FAMILY_UNIX_SOCKET,
                };

                /**
                 * @enum optionEnum defines connection for socket
                 */
                enum optionEnum {
                    OPTION_KEEP_ALIVE = 1,                  ///< Keeps  connections  active by enabling the periodic transmission of messages, if this is supported by the protocol.
                    OPTION_REUSE_ADDRESS,                   ///< Should allow reuse of local addresses[it's accepted by default].
                    OPTION_DONOT_USE_GATEWAY,               ///< Requests  that outgoing messages bypass the standard routing facilities.
                    OPTION_BROADCAST,                       ///< Permits  sending of broadcast messages, if this is supported by the protocol.
                    OPTION_OOB_INLINE,                      ///< Out-of-band(marked urgent) data keep inline in recieve operation.
                };

                /**
                 * @enum lingerOptionEnum defines linger connection for socket
                 */
                enum lingerOptionEnum {
                    LINGER_OPTION_GRACEFUL_CLOSE,           ///< close returns immediately, but any unsent data is transmitted (after close returns).
                    LINGER_OPTION_HARD_CLOSE,               ///< close returns immediately, and any unsent data is discarded.
                    LINGER_OPTION_WAIT_CLOSE,               ///< (*default*) close does not return until all unsent data is transmitted (or the connection is closed by the remote system).
                };

                /**
                 * set connection options
                 * @param option defines option that will be applied to the socket, @see network::connection::optionEnum
                 * @param flag defines state of option
                 */
                void setOption(short option,
                               bool  flag);

                /**
                 * @return true if socket option is set, @see network::connection::optionEnum
                 */
                bool isSetOption(int option) const;

                /**
                 * set linger option
                 * @param option is linger option, @see network::connection::lingerOptionEnum
                 * @param seconds how long to wait
                 * @note for CONNECTION_LINGEROPTION_WAIT_CLOSE only
                 */
                void setLingerOption(short option,
                                     int   seconds = 1);

                /**
                 * @return linger option that was set, @see network::connection::lingerOptionEnum
                 */
                short lingerOption() const;

                /**
                 * @return amount of seconds to wait
                 * @note for CONNECTION_LINGEROPTION_WAIT_CLOSE only
                 */
                int lingerPeriod() const;

                /**
                 * set input buffer size
                 * @param bytes defines size of the buffer in bytes
                 */
                void setInBufferSize(unsigned long bytes);

                /**
                 * set output buffer size
                 * @param bytes defines size of the buffer in bytes
                 */
                void setOutBufferSize(unsigned long bytes);

                /**
                 * @return input buffer size in bytes
                 */
                unsigned long inBufferSize() const;

                /**
                 * @return output buffer size in bytes
                 */
                unsigned long outBufferSize() const;

                /**
                 * set input timeout value of socket
                 * @param microseconds defines amount of time of timeout
                 */
                void setInTimeout(unsigned long microseconds);

                /**
                 * set output timeout value of socket
                 * @param microseconds defines amount of time of timeout
                 */
                void setOutTimeout(unsigned long microseconds);

                /**
                 * @return input timeout value
                 */
                unsigned long inTimeout() const;

                /**
                 * @return output timeout value
                 */
                unsigned long outTimeout() const;

                /**
                 * @return true if socked is blocked
                 */
                virtual bool isBlocked() const;

                /**
                 * block/unblock socket
                 * @param flag defines block statement
                 */
                virtual void block(bool flag);

              protected:

                /**
                 * close socket connection
                 * @param socket defines socket descriptor
                 */
                void _close(int socket);

                int socketOpts;                         ///< socket options

                short lingerOpts;                       ///< socket linger option
                int lingerSeconds;                      ///< socket linger timeout

                unsigned long inSocketTimeout;          ///< incomming operation timeout of socket; in microseconds
                unsigned long outSocketTimeout;         ///< outgoing operation timeout of socket; in microseconds

                unsigned long inSocketBufferSize;       ///< incoming buffer size of socket; in bytes
                unsigned long outSocketBufferSize;      ///< outgoing buffer size of socket; in bytes

                int socket;                             ///< socket descriptor

                bool blocked;                           ///< if true socket is blocked
            };
        };
    };
};
#endif
