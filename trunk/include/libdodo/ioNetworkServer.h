/***************************************************************************
 *            ioNetworkServer.h
 *
 *  Thu Oct 04 02:02:24 2005
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

#ifndef _IONETWORKSERVER_H_
#define _IONETWORKSERVER_H_

#include <libdodo/directives.h>

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

#include <libdodo/toolsFilesystem.h>
#include <libdodo/ioNetworkServerEx.h>
#include <libdodo/types.h>
#include <libdodo/ioNetworkOptions.h>
#include <libdodo/ioNetworkExchange.h>
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
				class server;
			};

			/**
			 * @enum ServerOperationTypeEnum defines type of operation for hook
			 */
			enum serverOperationTypeEnum
			{
				SERVER_OPERATION_CONNECT = 128,
				SERVER_OPERATION_CONNECTFROM,
				SERVER_OPERATION_CONNECT_UNIX,
				SERVER_OPERATION_BINDNLISTEN,
				SERVER_OPERATION_BINDNLISTEN_UNIX,
				SERVER_OPERATION_ACCEPT,
			};

#ifndef IO_WO_XEXEC

			/**
			 * @struct __xexecIoNetworkServerCollectedData defines data that could be retrieved from class(to modificate)[contains references]
			 */
			struct __xexecIoNetworkServerCollectedData
			{
				/**
				 * constructor
				 */
				__xexecIoNetworkServerCollectedData(int &operType,
													void *executor);

				int &operType;                                  ///< xexec operation

				void *executor;                                 ///< class that executed hook
			};

#endif

			/**
			 * @class Server provides network connection interface
			 */
			class server : public options,
						   virtual public nonBlockedAccessInfo
#ifndef IO_WO_XEXEC
						   ,
						   public xexec
#endif
			{
				friend class exchange;
				friend class ssl::server;

				private:

					/**
					 * copy constructor
					 * @note to prevent copying
					 */
					server(server &fs);

				public:

					/**
					 * constructor
					 * @param family defines family of the socket[see ServerOptionsProtoFamilyEnum]
					 * @param type defines type of the socket[see ServerOptionsTransferTypeEnum]
					 */
					server(short family, short type);

					/**
					 * destructor
					 */
					virtual ~server();

					/**
					 * bind to address and start to listen
					 * @param host defines local ip address to listen
					 * @param port defines local port to listen
					 * @param numberOfConnections defines the maximum length the queue of pending connections may grow to
					 * @note host can be '*' to specify all interfaces on the box
					 */
					virtual void bindNListen(const dodoString &host, int port, int numberOfConnections);

					/**
					 * bind to unix socket and start to listen
					 * @param path defines path to unix socket
					 * @param numberOfConnections defines the maximum length the queue of pending connections may grow to
					 * @param force defines if unix socket should be deleted if it exists
					 * @note host can be '*' to specify all interfaces on the box
					 */
					virtual void bindNListen(const dodoString &path, int numberOfConnections, bool force = false);

					/**
					 * accept incoming connections
					 * @return true on new connection acceptance
					 * @param init defines object that will be filled with info that may init exchange object
					 * @param info defines info about remote host
					 * @note for OPTIONS_TRANSFER_TYPE_DATAGRAM true is always returned
					 * for OPTIONS_PROTO_FAMILY_UNIX_SOCKET `info` will be always empty
					 */
					virtual bool accept(__initialAccept &init, __connInfo &info);

					/**
					 * accept incoming connections
					 * @return true on new connection acceptance
					 * @param init defines object that will be filled with info that may init exchange object
					 * @note for OPTIONS_TRANSFER_TYPE_DATAGRAM true is always returned
					 * for OPTIONS_PROTO_FAMILY_UNIX_SOCKET `info` will be always empty
					 */
					virtual bool accept(__initialAccept &init);

					bool blockInherited;                     ///< if true - children(exchange objects) become unblocked, if parent(Server) in unblocked; false by default

					/**
					 * @return descriptor of input stream
					 */
					virtual int getInDescriptor() const;

					/**
					 * @return descriptor of output stream
					 */
					virtual int getOutDescriptor() const;

				protected:

					short family;                                       ///< socket family
					short type;                                         ///< socket type

					/**
					 * restore options on connect/bind
					 */
					virtual void restoreOptions();

					/**
					 * create socket
					 */
					virtual void makeSocket();

					dodoString unixSock;                        ///< path to unix socket

#ifndef IO_WO_XEXEC

					__xexecIoNetworkServerCollectedData collectedData;                       ///< data collected for xexec

#endif
			};
		};
	};
};

#endif
