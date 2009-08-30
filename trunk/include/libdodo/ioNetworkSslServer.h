/***************************************************************************
 *            ioNetworkSslServer.h
 *
 *  Tue Jun 10 16:45:47 2008
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

#ifndef _IONETWORKSSLSERVER_H_
#define _IONETWORKSSLSERVER_H_ 1

#include <libdodo/directives.h>

#ifdef OPENSSL_EXT
#include <libdodo/ioNetworkServer.h>
#include <libdodo/ioNetworkSslExchange.h>
#include <libdodo/types.h>

namespace dodo {
	namespace io {
		namespace ssl {
			struct __context__;
			struct __certificates__;
		};

		namespace network {
			namespace ssl {
				/**
				 * @class server
				 * @brief provides SSL network connection interface
				 */
				class server : public network::server {
					friend class exchange;

				  private:

					/**
					 * copy constructor
					 * @note to prevent copying
					 */
					server(server &);

				  public:

					/**
					 * constructor
					 * @param family defines family of the socket, @see connection::protocolFamilyEnum
					 * @param type defines type of the socket, @see connection::transferEnum
					 */
					server(short family,
						   short type);

					/**
					 * destructor
					 */
					virtual ~server();

					/**
					 * set certificates information
					 * @param certs defines certificates information
					 */
					void setSertificates(const io::ssl::__certificates__ &certs);

					/**
					 * remove certificates information
					 */
					void removeSertificates();

					/**
					 * bind to address and start to listen
					 * @param host defines local ip address to listen
					 * @param port defines local port to listen
					 * @param numberOfConnections defines the maximum length the queue of pending connections may grow to
					 * @note host can be '*' to specify all interfaces on the box
					 */
					virtual void serve(const dodoString &host,
									   int              port,
									   int              numberOfConnections);

					/**
					 * bind to unix socket and start to listen
					 * @param path defines path to unix socket
					 * @param numberOfConnections defines the maximum length the queue of pending connections may grow to
					 * @param force defines if unix socket should be deleted if it exists
					 * @note host can be '*' to specify all interfaces on the box
					 */
					virtual void serve(const dodoString &path,
									   int              numberOfConnections,
									   bool             force = false);

					/**
					 * accept incoming connections
					 * @return true on new connection acceptance
					 * @param init defines object that will be filled with info that may init exchange object
					 * @param info defines info about remote host
					 * @note for OPTIONS_TRANSFER_DATAGRAM true is always returned
					 * for OPTIONS_PROTO_FAMILY_UNIX_SOCKET `info` will be always empty
					 */
					virtual bool accept(network::exchange::__init__ &init,
										__peer__      &info);

					/**
					 * accept incoming connections
					 * @return true on new connection acceptance
					 * @param init defines object that will be filled with info that may init exchange object
					 * @note for OPTIONS_TRANSFER_DATAGRAM true is always returned
					 * for OPTIONS_PROTO_FAMILY_UNIX_SOCKET `info` will be always empty
					 */
					virtual bool accept(network::exchange::__init__ &init);

				  protected:

					/**
					 * initialize SSL objects
					 */
					void initSsl();

					/**
					 * accept SSL connection
					 * @param init defines object that will be filled with info that may init exchange object
					 */
					void acceptSsl(exchange::__init__ &init);

					io::ssl::__context__ *ctx; ///< SSL context
				};
			};
		};
	};
};
#endif
#endif
