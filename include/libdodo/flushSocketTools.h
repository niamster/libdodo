/***************************************************************************
 *            flushSocket.h
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

#ifndef _FLUSHSOCKETTOOLS_H_
#define _FLUSHSOCKETTOOLS_H_

#include <directives.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <flushSocketToolsEx.h>
#include <types.h>
#include <tools.h>

namespace dodo
{

	/**
	 * @struct __ifInfo describes interface info
	 */
	struct __ifInfo
	{
		dodoString address;     ///< ip address of the interface
		dodoString broadcast;   ///< broadcast address of the interface
		dodoString netmask;     ///< netmask of the interface
		dodoString hwaddr;      ///< harware address of the interface(MAC)

		bool up;                ///< true if interface is up
		bool loop;              ///< true is a loopback
	};

	/**
	 * @struct __hostInfo describes info about host
	 */
	struct __hostInfo
	{
		dodoString name;            ///< original name of the host
		dodoStringArr aliases;      ///< aliases of the host
		dodoStringArr addresses;    ///< addresses of the host
	};

	/**
	 * @struct __servInfo describes info about service
	 */
	struct __servInfo
	{
		dodoString name;        ///< original name of the service
		dodoStringArr aliases;  ///< sevice's aliases
		int port;               ///< port of service
	};

	/**
	 * @class flushSocket performs communication actions!!
	 * exchange of data is flushSocketExchange class' task; ou init it with connect or accept methods
	 */
	class flushSocketTools

	{

		public:

		/**
		 * @return a list of interfaces in system
		 */
		static dodoStringArr getInterfacesNames();

		/**
		 * @return info about interface
		 * @param interface is name of interface about what info is needed
		 */
		static __ifInfo getInterfaceInfo(const dodoString &interface);

		/**
		 * @return info about given host
		 * @param host points to host about what info would be given
		 */
		static __hostInfo getHostInfo(const dodoString &host);

		/**
		 * @return name of local host
		 */
		static dodoString getLocalName();

		/**
		 * sets local name
		 * @param host is new  name of the host
		 */
		static void
		setLocalName(const dodoString &host);

		/**
		 * @return info about service
		 * @param service is service name
		 * @param protocol specifies protocol of service(tcp, udp ..)
		 */
		static __servInfo getServiceInfo(const dodoString &service, const dodoString &protocol);

		/**
		 * @return info about service
		 * @param port is port of service
		 * @param protocol specifies protocol of service(tcp, udp ..)
		 */
		static __servInfo getServiceInfo(int port, const dodoString &protocol);

	};

};

#endif
