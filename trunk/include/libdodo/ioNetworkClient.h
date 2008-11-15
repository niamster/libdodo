/***************************************************************************
 *            ioNetworkClient.h
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

#ifndef _IONETWORKCLIENT_H_
#define _IONETWORKCLIENT_H_ 1

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
#include <libdodo/ioNetworkClientEx.h>
#include <libdodo/types.h>
#include <libdodo/ioNetworkConnection.h>
#include <libdodo/ioNetworkExchange.h>
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
				class client;
			};

			/**
			 * @enum clientOperationTypeEnum defines type of operation for hook
			 */
			enum clientOperationTypeEnum
			{
				CLIENT_OPERATION_CONNECT = 128,
				CLIENT_OPERATION_CONNECTFROM,
				CLIENT_OPERATION_CONNECT_UNIX,
				CLIENT_OPERATION_BINDNLISTEN,
				CLIENT_OPERATION_BINDNLISTEN_UNIX,
				CLIENT_OPERATION_ACCEPT,
			};

#ifndef IO_WO_XEXEC

			/**
			 * @class __xexecIoNetworkClientCollectedData
			 * @brief defines data that could be retrieved from class(to modificate)[contains references]
			 */
			class __xexecIoNetworkClientCollectedData : public __xexecCollectedData
			{
				public:

					/**
					 * constructor
					 * @param executor defines class that executed hook
					 * @param execObject defines type of object that executed a hook[see xexecObjectTypeEnum]
					 */
					__xexecIoNetworkClientCollectedData(xexec *executor, short execObject);
			};

#endif

			/**
			 * @class client
			 * @brief provides network connection interface
			 */
			class client : public connection
#ifndef IO_WO_XEXEC
						   ,
						   public xexec
#endif
			{
				friend class exchange;
				friend class ssl::client;

				private:

					/**
					 * copy constructor
					 * @note to prevent copying
					 */
					client(client &fs);

				public:

					/**
					 * constructor
					 * @param family defines family of the socket[see connectionProtoFamilyEnum]
					 * @param type defines type of the socket[see connectionTransferTypeEnum]
					 */
					client(short family, short type);

					/**
					 * destructor
					 */
					virtual ~client();

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

					bool blockInherited;                                         ///< if true - children(exchange objects) become unblocked, if parent(Client) in unblocked; false by default

				protected:

					short family;                                                           ///< socket family
					short type;                                                             ///< socket type

					/**
					 * restore options on connect/bind
					 */
					virtual void restoreOptions();

					/**
					 * create socket
					 */
					virtual void makeSocket();

					dodoString unixSock;                                            ///< path to unix socket

#ifndef IO_WO_XEXEC

					__xexecIoNetworkClientCollectedData collectedData;                                           ///< data collected for xexec

#endif
			};
		};
	};

};

#endif
