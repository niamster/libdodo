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
#define _IONETWORKSSLSERVER_H_

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
#include <libdodo/ioNetworkSslServerEx.h>
#include <libdodo/ioNetworkServer.h>
#include <libdodo/ioSsl.h>
#include <libdodo/types.h>
#include <libdodo/ioNetworkSslExchange.h>
#include <libdodo/xexec.h>
#include <libdodo/ioEventInfo.h>

namespace dodo
{
	namespace io
	{
		namespace network
		{
			namespace ssl
			{
				/**
				 * @class server
				 * @brief provides SSL network connection interface
				 */
				class server : public network::server
				{
					friend class exchange;

					private:

						/**
						 * copy constructor
						 * @note to prevent copying
						 */
						server(server &fs);

					public:

						/**
						 * constructor
						 * @param family defines family of the socket[see connectionProtoFamilyEnum]
						 * @param type defines type of the socket[see connectionTransferTypeEnum]
						 */
						server(short family, short type);

						/**
						 * destructor
						 */
						virtual ~server();

						/**
						 * set certificates information
						 * @param certs defines certificates information
						 */
						virtual void setSertificates(const io::ssl::__certificates &certs);

						/**
						 * remove certificates information
						 */
						virtual void removeSertificates();

						/**
						 * bind to address and start to listen
						 * @param host defines local ip address to listen
						 * @param port defines local port to listen
						 * @param numberOfConnections defines the maximum length the queue of pending connections may grow to
						 * @note host can be '*' to specify all interfaces on the box
						 */
						virtual void serve(const dodoString &host, int port, int numberOfConnections);

						/**
						 * bind to unix socket and start to listen
						 * @param path defines path to unix socket
						 * @param numberOfConnections defines the maximum length the queue of pending connections may grow to
						 * @param force defines if unix socket should be deleted if it exists
						 * @note host can be '*' to specify all interfaces on the box
						 */
						virtual void serve(const dodoString &path, int numberOfConnections, bool force = false);

						/**
						 * accept incoming connections
						 * @return true on new connection acceptance
						 * @param init defines object that will be filled with info that may init exchange object
						 * @param info defines info about remote host
						 * @note for OPTIONS_TRANSFER_TYPE_DATAGRAM true is always returned
						 * for OPTIONS_PROTO_FAMILY_UNIX_SOCKET `info` will be always empty
						 */
						virtual bool accept(__initialAccept &init, __peerInfo &info);

						/**
						 * accept incoming connections
						 * @return true on new connection acceptance
						 * @param init defines object that will be filled with info that may init exchange object
						 * @note for OPTIONS_TRANSFER_TYPE_DATAGRAM true is always returned
						 * for OPTIONS_PROTO_FAMILY_UNIX_SOCKET `info` will be always empty
						 */
						virtual bool accept(__initialAccept &init);

					protected:

						/**
						 * initialize SSL objects
						 */
						virtual void initSsl();

						/**
						 * accept SSL connection
						 * @param init defines object that will be filled with info that may init exchange object
						 */
						virtual void acceptSsl(__initialAccept &init);

						SSL_CTX *sslCtx;                                                        ///< SSL context
				};
			};
		};
	};
};

#endif

#endif
