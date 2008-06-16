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
				 * @enum clientOperationTypeEnum defines type of operation for hook
				 */
				enum clientOperationTypeEnum
				{
					CLIENT_OPERATION_CONNECT,
					CLIENT_OPERATION_CONNECTFROM,
					CLIENT_OPERATION_CONNECT_UNIX,
					CLIENT_OPERATION_BINDNLISTEN,
					CLIENT_OPERATION_BINDNLISTEN_UNIX,
					CLIENT_OPERATION_ACCEPT,
				};

#ifndef IONETWORKSSLCLIENT_WO_XEXEC

				/**
				 * @struct __xexexIoCollectedData defines data that could be retrieved from class(to modificate)[contains references]
				 */
				struct __xexexIoNetworkClientCollectedData
				{
					/**
					 * constructor
					 */
					__xexexIoNetworkClientCollectedData(int &operType,
														void *executor);

					int &operType;                                      ///< xexec operation

					void *executor;                                     ///< class that executed hook
				};

#endif

				/**
				 * @class client provides network connection interface
				 */
				class client : public xexec,
							   public options
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
						 * connect from specific address
						 * @param local defines ip address to bind
						 * @param destinaton defines destinaton ip address/port of host to connect
						 * @param exchange defines an oject that will perform I/O operations
						 */
						virtual void connectFrom(const dodoString &local, const __connInfo &destinaton, exchange &exchange);

						/**
						 * connect
						 * @param host defines ip address of host to connect
						 * @param port defines port of host to connect
						 * @param exchange defines an oject that will perform I/O operations
						 */
						virtual void connect(const dodoString &host, int port, exchange &exchange);

						/**
						 * connect
						 * @param destinaton defines destinaton ip address/port of host to connect
						 * @param exchange defines an oject that will perform I/O operations
						 */
						virtual void connect(const __connInfo &destinaton, exchange &exchange);

						/**
						 * connect
						 * @param path defines path to unix socket
						 * @param exchange defines an oject that will perform I/O operations
						 */
						virtual void connect(const dodoString &path, exchange &exchange);

						bool blockInherited;                         ///< if true - children(exchange objects) become unblocked, if parent(Client) in unblocked; false by default

					protected:

						short family;                                           ///< socket family
						short type;                                             ///< socket type

						/**
						 * restore options on connect/bind
						 */
						virtual void restoreOptions();

						/**
						 * create socket
						 */
						virtual void makeSocket();

						/**
						 * initialize SSL objects
						 */
						virtual void initSsl();

						/**
						 * establish SSL connection
						 */
						virtual void connectSsl();


						dodoString unixSock;                            ///< path to unix socket

						SSL_CTX *sslCtx;                                ///< SSL context
						SSL *sslHandle;                                 ///< SSL connection handle

						bool sslConnected;                              ///< true if SSL connection established

#ifndef IONETWORKSSLCLIENT_WO_XEXEC

						__xexexIoNetworkClientCollectedData collectedData;                           ///< data collected for xexec

#endif
				};
			};
		};
	};

};

#endif

#endif
