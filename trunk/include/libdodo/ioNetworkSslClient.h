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
#define _IONETWORKSSLCLIENT_H_

#include <libdodo/directives.h>

#ifdef OPENSSL_EXT

#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <openssl/ssl.h>

#include <libdodo/toolsFilesystem.h>
#include <libdodo/ioNetworkSslClientEx.h>
#include <libdodo/ioNetworkClient.h>
#include <libdodo/ioNetworkSsl.h>
#include <libdodo/types.h>
#include <libdodo/ioNetworkOptions.h>
#include <libdodo/ioNetworkSslExchange.h>
#include <libdodo/xexec.h>
#include <libdodo/ioNonBlockedAccessInfo.h>

namespace dodo
{
	namespace io
	{
		namespace network
		{
			namespace ssl
			{
				/**
				 * @class client provides network connection interface
				 */
				class client : public network::client
				{
					friend class exchange;

					private:

						/**
						 * connstructor
						 * to prevent copying
						 */
						client(client &fs);

					public:

						/**
						 * constructor
						 * @param family defines family of the socket[see ClientOptionsProtoFamilyEnum]
						 * @param type defines type of the socket[see ClientOptionsTransferTypeEnum]
						 */
						client(short family, short type);

						/**
						 * destructor
						 */
						virtual ~client();
						
						/**
						 * set certificates information
						 * @param certs defines certificates information 
						 */
						virtual void setSertificates(const __certificates &certs);

						/**
						 * connect from specific address
						 * @param local defines ip address to bind
						 * @param host defines ip address of host to connect
						 * @param port defines port of host to connect
						 * @param exchange defines an oject that will perform I/O operations
						 */
						virtual void connectFrom(const dodoString &local, const dodoString &host, int port, exchange &exchange);

						/**
						 * connect
						 * @param host defines ip address of host to connect
						 * @param port defines port of host to connect
						 * @param exchange defines an oject that will perform I/O operations
						 */
						virtual void connect(const dodoString &host, int port, exchange &exchange);

						/**
						 * connect
						 * @param path defines path to unix socket
						 * @param exchange defines an oject that will perform I/O operations
						 */
						virtual void connect(const dodoString &path, exchange &exchange);

					protected:

						/**
						 * initialize SSL objects
						 */
						virtual void initSsl();

						/**
						 * establish SSL connection
						 */
						virtual void connectSsl();

						SSL_CTX *sslCtx;                                ///< SSL context
						SSL *sslHandle;                                 ///< SSL connection handle

						bool sslConnected;                              ///< true if SSL connection established
				};
			};
		};
	};

};

#endif

#endif
