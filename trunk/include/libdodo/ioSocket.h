/***************************************************************************
 *            ioSocket.h
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

#ifndef _IOSOCKET_H_
#define _IOSOCKET_H_

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
#include <libdodo/tools.h>
#include <libdodo/ioSocketEx.h>
#include <libdodo/types.h>
#include <libdodo/ioSocketOptions.h>
#include <libdodo/ioSocketExchange.h>
#include <libdodo/xexec.h>
#include <libdodo/ioNBAInfo.h>

namespace dodo
{

	/**
	 * @enum ioSocketOperationTypeEnum defines type of operation for hook
	 */
	enum ioSocketOperationTypeEnum
	{
		IOSOCKET_OPERATION_CONNECT,
		IOSOCKET_OPERATION_CONNECTFROM,
		IOSOCKET_OPERATION_CONNECT_UNIX,
		IOSOCKET_OPERATION_BINDNLISTEN,
		IOSOCKET_OPERATION_BINDNLISTEN_UNIX,
		IOSOCKET_OPERATION_ACCEPT,
	};

	/**
	 * @struct __connInfo defines connection information
	 */
	struct __connInfo
	{
		dodoString host;///< host of the box 
		int port;///< port of the box
	};

	/**
	 * @struct __xexexIoCollectedData defines data that could be retrieved from class(to modificate)[contains references]
	 */
	struct __xexexIoSocketCollectedData
	{
		/**
		 * constructor
		 */
		__xexexIoSocketCollectedData(int &operType,
							   		void *executor);

		int &operType;              ///< xexec operation 

		void *executor;             ///< class that executed hook
	};
	
	/**
	 * @class ioSocket provides network connection interface
	 */
	class ioSocket : public xexec, 
					public ioSocketOptions,
					virtual public ioNBAInfo
	{
			friend class ioSocketExchange;

		private:

			/**
			 * connstructor
			 * to prevent copying
			 */
			ioSocket(ioSocket &fs);

		public:

			/**
			 * constructor
			 * @param server defines type of service; if true as a server
			 * @param family defines family of the socket[see socketProtoFamilyEnum]
			 * @param type defines type of the socket[see socketTransferTypeEnum]
			 */
			ioSocket(bool server, short family, short type);

			/**
			 * destructor
			 */
			virtual ~ioSocket();

#ifndef IOSOCKET_WO_XEXEC

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
			 * @param obj defines object that called the hook
			 * @param type defines hook type[see xexecObjTypeEnum]
			 * @param data defines hook data
			 * @param toInit defines data that will be passed to the init function
			 * @note type of hook[pre/post] is defined in module
			 */
			virtual __xexecCounts addExec(const dodoString &path, void *data, void *toInit = NULL);

			/**
			 * set function that will be executed after the main action call
			 * @return id of the hook method
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param obj defines object that called the hook
			 * @param type defines hook type[see xexecObjTypeEnum]
			 * @param data defines hook data
			 * @param toInit defines data that will be passed to the init function
			 */
			virtual int addPostExec(const dodoString &module, void *data, void *toInit = NULL);

			/**
			 * set function that will be executed before the main action call
			 * @return id of the hook method
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param obj defines object that called the hook
			 * @param type defines hook type[see xexecObjTypeEnum]
			 * @param data defines hook data
			 * @param toInit defines data that will be passed to the init function
			 */
			virtual int addPreExec(const dodoString &module, void *data, void *toInit = NULL);

#endif

#endif

			/**
			 * connect from specific address
			 * @param local defines ip address to bind
			 * @param host defines ip address of host to connect
			 * @param port defines port of host to connect
			 * @param exchange defines an oject that will perform I/O operations
			 */
			virtual void connectFrom(const dodoString &local, const dodoString &host, int port, ioSocketExchange &exchange);

			/**
			 * connect from specific address
			 * @param local defines ip address to bind
			 * @param destinaton defines destinaton ip address/port of host to connect
			 * @param exchange defines an oject that will perform I/O operations
			 */
			virtual void connectFrom(const dodoString &local, const __connInfo &destinaton, ioSocketExchange &exchange);

			/**
			 * connect
			 * @param host defines ip address of host to connect
			 * @param port defines port of host to connect
			 * @param exchange defines an oject that will perform I/O operations
			 */
			virtual void connect(const dodoString &host, int port, ioSocketExchange &exchange);

			/**
			 * connect
			 * @param destinaton defines destinaton ip address/port of host to connect
			 * @param exchange defines an oject that will perform I/O operations
			 */
			virtual void connect(const __connInfo &destinaton, ioSocketExchange &exchange);

			/**
			 * connect
			 * @param path defines path to unix socket
			 * @param exchange defines an oject that will perform I/O operations
			 */
			virtual void connect(const dodoString &path, ioSocketExchange &exchange);

			/**
			 * bind to address and start to listen
			 * @param host defines local ip address to listen
			 * @param port defines local port to listen
			 * @param numberOfConnections defines the maximum length the queue of pending connections may grow to
			 * @note host can be '*' to specify all interfaces on the box
			 */
			virtual void bindNListen(const dodoString &host, int port, int numberOfConnections);

			/**
			 * bind to address and start to listen
			 * @param destinaton defines local ip address/port of host to connect
			 * @param numberOfConnections defines the maximum length the queue of pending connections may grow to
			 * @note host can be '*' to specify all interfaces on the box
			 */
			virtual void bindNListen(const __connInfo &destinaton, int numberOfConnections);

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
			 * @param init defines object that will be filled with info that may init ioSocketExchange object
			 * @param info defines info about remote host
			 * @note for IOSOCKETOPTIONS_TRANSFER_TYPE_DATAGRAM true is always returned 
			 * for IOSOCKETOPTIONS_PROTO_FAMILY_UNIX_SOCKET `info` will be always empty
			 */
			virtual bool accept(__initialAccept &init, __connInfo &info);

			/**
			 * accept incoming connections
			 * @return true on new connection acceptance
			 * @param init defines object that will be filled with info that may init ioSocketExchange object
			 * @note for IOSOCKETOPTIONS_TRANSFER_TYPE_DATAGRAM true is always returned 
			 * for IOSOCKETOPTIONS_PROTO_FAMILY_UNIX_SOCKET `info` will be always empty
			 */
			virtual bool accept(__initialAccept &init);

			bool blockInherited; ///< if true - children(ioSocketExchange objects) become unblocked, if parent(ioSocket) in unblocked; false by default

			/**
			 * @return descriptor of input stream
			 */
			virtual int getInDescriptor() const;

			/**
			 * @return descriptor of output stream
			 */
			virtual int getOutDescriptor() const;

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
			
			__xexexIoSocketCollectedData collectedData;   ///< data collected for xexec
	};

};

#endif
