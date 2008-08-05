/***************************************************************************
 *            toolsNetwork.h
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

#ifndef _TOOLSNETWORK_H_
#define _TOOLSNETWORK_H_

#include <libdodo/directives.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <libdodo/toolsNetworkEx.h>
#include <libdodo/types.h>
#include <libdodo/toolsMisc.h>

namespace dodo
{
	namespace tools
	{
		/**
		 * @struct __interfaceInfo defines interface information
		 */
		struct __interfaceInfo
		{
			dodoString address;                             ///< ip address of the interface
			dodoString broadcast;                           ///< broadcast address of the interface
			dodoString netmask;                             ///< netmask of the interface
			dodoString hwaddr;                              ///< harware address of the interface(MAC)

			bool up;                                        ///< true if interface is up
			bool loop;                                      ///< true if interface is a loopback
		};

		/**
		 * @struct __hostInfo defines information about host
		 */
		struct __hostInfo
		{
			dodoString name;                                        ///< original name of the host
			dodoStringArray aliases;                                ///< aliases of the host
			dodoStringArray addresses;                              ///< addresses of the host
		};

		/**
		 * @struct __serviceInfo defines info about service
		 */
		struct __serviceInfo
		{
			dodoString name;                                        ///< original name of the service
			dodoStringArray aliases;                                ///< aliases of the service
			int port;                                               ///< port of the service
		};

		/**
		 * @class network provides information about network environment
		 */
		class network
		{

			public:

				/**
				 * @return a list of interfaces
				 */
				static dodoStringArray getInterfacesNames();

				/**
				 * @return information about the interface
				 * @param interface defines a name of the interface
				 */
				static __interfaceInfo getInterfaceInfo(const dodoString &interface);

				/**
				 * @return information about the given host
				 * @param host defines a name of the host
				 */
				static __hostInfo getHostInfo(const dodoString &host);

				/**
				 * @return primary host ip
				 * @param host defines a name of the host
				 */
				static dodoString getHostPrimaryIp(const dodoString &host);

				/**
				 * @return name of the local host
				 */
				static dodoString getLocalName();

				/**
				 * set local host name
				 * @param host defines name of the host
				 */
				static void setLocalName(const dodoString &host);

				/**
				 * @return information about the service
				 * @param service defices name of the service
				 * @param protocol defines protocol of the service(tcp, udp ..)
				 */
				static __serviceInfo getServiceInfo(const dodoString &service, const dodoString &protocol);

				/**
				 * @return information about the service
				 * @param port defices port of the service
				 * @param protocol defines protocol of the service(tcp, udp ..)
				 */
				static __serviceInfo getServiceInfo(int port, const dodoString &protocol);

		};
	};
};

#endif
