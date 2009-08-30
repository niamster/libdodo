/***************************************************************************
 *            ioNetworkSslClient.h
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

#ifndef _IONETWORKSSLCLIENT_H_
#define _IONETWORKSSLCLIENT_H_ 1

#include <libdodo/directives.h>

#ifdef OPENSSL_EXT
#include <libdodo/ioNetworkClient.h>
#include <libdodo/types.h>

namespace dodo {
	namespace io {
		namespace network {
			namespace http {
				class client;
			};
		};

		namespace ssl {
			struct __connection__;
			struct __context__;
			struct __certificates__;
		};

		namespace network {
			namespace ssl {
				class exchange;

				/**
				 * @class client
				 * @brief provides network SSL connection interface
				 */
				class client : public network::client {
					friend class exchange;
					friend class network::http::client;

				  private:

					/**
					 * copy constructor
					 * @note to prevent copying
					 */
					client(client &);

				  public:

					/**
					 * constructor
					 * @param family defines family of the socket, @see connection::protocolFamilyEnum
					 * @param type defines type of the socket, @see connection::transferEnum
					 */
					client(short family,
						   short type);

					/**
					 * destructor
					 */
					virtual ~client();

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
					 * connect from specific address
					 * @param local defines ip address to bind
					 * @param host defines ip address of host to connect
					 * @param port defines port of host to connect
					 * @param exchange defines an oject that will perform I/O operations
					 */
					virtual void connectFrom(const dodoString &local,
											 const dodoString &host,
											 int              port,
											 network::exchange         &exchange);

					/**
					 * connect
					 * @param host defines ip address of host to connect
					 * @param port defines port of host to connect
					 * @param exchange defines an oject that will perform I/O operations
					 */
					virtual void connect(const dodoString &host,
										 int              port,
										 network::exchange         &exchange);

					/**
					 * connect
					 * @param path defines path to unix socket
					 * @param exchange defines an oject that will perform I/O operations
					 */
					virtual void connect(const dodoString &path,
										 network::exchange         &exchange);

				  protected:

					/**
					 * initialize SSL objects
					 */
					void initSsl();

					/**
					 * establish SSL connection
					 */
					void connectSsl();

					io::ssl::__connection__ *handle; ///< ssl connection handle
					io::ssl::__context__ *ctx;       ///< ssl connection context

					bool sslConnected;                  ///< true if SSL connection established
				};
			};
		};
	};
};
#endif
#endif
