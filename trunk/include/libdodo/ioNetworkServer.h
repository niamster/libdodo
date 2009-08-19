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
#define _IONETWORKSERVER_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/ioNetworkConnection.h>
#include <libdodo/xexec.h>
#include <libdodo/ioEventInfo.h>

namespace dodo {
	namespace io {
		namespace network {
			namespace ssl {
				class server;
			};

			class __initialAccept__;

			/**
			 * @enum serverOperationTypeEnum defines type of operation for hook
			 */
			enum serverOperationTypeEnum {
				SERVER_OPERATION_CONNECT = 128,
				SERVER_OPERATION_CONNECTFROM,
				SERVER_OPERATION_CONNECT_UNIX,
				SERVER_OPERATION_BINDNLISTEN,
				SERVER_OPERATION_BINDNLISTEN_UNIX,
				SERVER_OPERATION_ACCEPT,
			};

#ifndef IO_WO_XEXEC
			/**
			 * @class __xexecIoNetworkServerCollectedData__
			 * @brief defines data that could be retrieved from class(to modificate)[contains references]
			 */
			class __xexecIoNetworkServerCollectedData__ : public __xexecCollectedData__ {
			  public:

				/**
				 * constructor
				 * @param executor defines class that executed hook
				 * @param execObject defines type of object that executed a hook[see xexecObjectTypeEnum]
				 */
				__xexecIoNetworkServerCollectedData__(xexec *executor, short execObject);
			};
#endif

			/**
			 * @class server
			 * @brief provides network connection interface
			 */
			class server : public connection,
						   virtual public eventInfo
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
				 * @param family defines family of the socket[see connectionProtoFamilyEnum]
				 * @param type defines type of the socket[see connectionTransferTypeEnum]
				 */
				server(short family,
					   short type);

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
				 * @note for OPTIONS_TRANSFER_TYPE_DATAGRAM true is always returned
				 * for OPTIONS_PROTO_FAMILY_UNIX_SOCKET `info` will be always empty
				 */
				virtual bool accept(__initialAccept__ &init,
									__peerInfo__      &info);

				/**
				 * accept incoming connections
				 * @return true on new connection acceptance
				 * @param init defines object that will be filled with info that may init exchange object
				 * @note for OPTIONS_TRANSFER_TYPE_DATAGRAM true is always returned
				 * for OPTIONS_PROTO_FAMILY_UNIX_SOCKET `info` will be always empty
				 */
				virtual bool accept(__initialAccept__ &init);

				bool blockInherited;                                    ///< if true - children(exchange objects) become unblocked, if parent(Server) in unblocked; false by default

			  protected:

				/**
				 * @return descriptor of input stream
				 */
				virtual int getInDescriptor() const;

				/**
				 * @return descriptor of output stream
				 */
				virtual int getOutDescriptor() const;

				short family;                                           ///< socket family
				short type;                                             ///< socket type

				/**
				 * restore options on connect/bind
				 */
				void restoreOptions();

				/**
				 * create socket
				 */
				void makeSocket();

				dodoString unixSock;                                    ///< path to unix socket

#ifndef IO_WO_XEXEC
				__xexecIoNetworkServerCollectedData__ collectedData;    ///< data collected for xexec
#endif
			};
		};
	};
};

#endif
