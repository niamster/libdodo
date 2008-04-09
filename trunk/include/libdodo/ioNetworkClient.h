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

#ifndef _IONETWORKCLIENT_H_
#define _IONETWORKCLIENT_H_

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

#include <libdodo/ioDiskTools.h>
#include <libdodo/ioNetworkClientEx.h>
#include <libdodo/types.h>
#include <libdodo/ioNetworkOptions.h>
#include <libdodo/ioNetworkExchange.h>
#include <libdodo/xexec.h>
#include <libdodo/ioNonBlockedAccessInfo.h>

namespace dodo
{

	/**
	 * @enum ioNetworkClientOperationTypeEnum defines type of operation for hook
	 */
	enum ioNetworkClientOperationTypeEnum
	{
		IONETWORKCLIENT_OPERATION_CONNECT,
		IONETWORKCLIENT_OPERATION_CONNECTFROM,
		IONETWORKCLIENT_OPERATION_CONNECT_UNIX,
		IONETWORKCLIENT_OPERATION_BINDNLISTEN,
		IONETWORKCLIENT_OPERATION_BINDNLISTEN_UNIX,
		IONETWORKCLIENT_OPERATION_ACCEPT,
	};

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

		int &operType;              ///< xexec operation 

		void *executor;             ///< class that executed hook
	};
	
	/**
	 * @class ioNetworkClient provides network connection interface
	 */
	class ioNetworkClient : public xexec, 
					public ioNetworkOptions
	{
			friend class ioNetworkExchange;

		private:

			/**
			 * connstructor
			 * to prevent copying
			 */
			ioNetworkClient(ioNetworkClient &fs);

		public:

			/**
			 * constructor
			 * @param server defines type of service; if true as a server
			 * @param family defines family of the socket[see ioNetworkClientOptionsProtoFamilyEnum]
			 * @param type defines type of the socket[see ioNetworkClientOptionsTransferTypeEnum]
			 */
			ioNetworkClient(short family, short type);

			/**
			 * destructor
			 */
			virtual ~ioNetworkClient();

#ifndef IONETWORKCLIENT_WO_XEXEC

			/**
			 * adds hook after the operation by callback
			 * @return id of the hook method
			 * @param func defines function that will be called
			 * @param data defines hook data
			 */
			virtual int addPostExec(inExec func, void *data);

			/**
			 * adds hook before the operation by callback
			 * @return id of the hook method
			 * @param func defines function that will be called
			 * @param data defines hook data
			 */
			virtual int addPreExec(inExec func, void *data);

#ifdef DL_EXT

			/**
			 * set function that will be executed before/after the main action call
			 * @return id of the hook method
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param data defines hook data
			 * @param toInit defines data that will be passed to the init function
			 * @note type of hook[pre/post] is defined in module
			 */
			virtual __xexecCounts addExec(const dodoString &path, void *data, void *toInit = NULL);

			/**
			 * set function that will be executed after the main action call
			 * @return id of the hook method
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param data defines hook data
			 * @param toInit defines data that will be passed to the init function
			 */
			virtual int addPostExec(const dodoString &path, void *data, void *toInit = NULL);

			/**
			 * set function that will be executed before the main action call
			 * @return id of the hook method
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param data defines hook data
			 * @param toInit defines data that will be passed to the init function
			 */
			virtual int addPreExec(const dodoString &path, void *data, void *toInit = NULL);

#endif

#endif
			/**
			 * connect from specific address
			 * @param local defines ip address to bind
			 * @param host defines ip address of host to connect
			 * @param port defines port of host to connect
			 * @param exchange defines an oject that will perform I/O operations
			 */
			virtual void connectFrom(const dodoString &local, const dodoString &host, int port, ioNetworkExchange &exchange);

			/**
			 * connect from specific address
			 * @param local defines ip address to bind
			 * @param destinaton defines destinaton ip address/port of host to connect
			 * @param exchange defines an oject that will perform I/O operations
			 */
			virtual void connectFrom(const dodoString &local, const __connInfo &destinaton, ioNetworkExchange &exchange);

			/**
			 * connect
			 * @param host defines ip address of host to connect
			 * @param port defines port of host to connect
			 * @param exchange defines an oject that will perform I/O operations
			 */
			virtual void connect(const dodoString &host, int port, ioNetworkExchange &exchange);

			/**
			 * connect
			 * @param destinaton defines destinaton ip address/port of host to connect
			 * @param exchange defines an oject that will perform I/O operations
			 */
			virtual void connect(const __connInfo &destinaton, ioNetworkExchange &exchange);

			/**
			 * connect
			 * @param path defines path to unix socket
			 * @param exchange defines an oject that will perform I/O operations
			 */
			virtual void connect(const dodoString &path, ioNetworkExchange &exchange);

			bool blockInherited; ///< if true - children(ioNetworkExchange objects) become unblocked, if parent(ioNetworkClient) in unblocked; false by default

		protected:

			/**
			 * restore options on connect/bind
			 */
			virtual void restoreOptions();

			/**
			 * create socket
			 */
			virtual void makeSocket();

			bool opened; ///< true if I/O session is opened

			bool server;            ///< true if type of service is server

			dodoString unixSock;    ///< path to unix socket
			
			__xexexIoNetworkClientCollectedData collectedData;   ///< data collected for xexec
	};

};

#endif
