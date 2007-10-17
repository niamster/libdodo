/***************************************************************************
 *            flushSocket.h
 *
 *  Thu Oct 04 02:02:24 2005
 *  Copyright  2005  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _FLUSHSOCKETTOOLS_H_
#define _FLUSHSOCKETTOOLS_H_

#include <directives.h>

#include <types.h>

namespace dodo
{	
	
	 /**
	  * @struct __ifInfo describes interface info
	  */
	 struct __ifInfo
	 {
	 	std::string address;///< ip address of the interface
	 	std::string broadcast;///< broadcast address of the interface
	 	std::string netmask;///< netmask of the interface
	 	std::string hwaddr;///< harware address of the interface(MAC)
	 	
	 	bool up;///< true if interface is up
	 	bool loop;///< true is a loopback
	 };
	 
	 /**
	  * @struct __hostInfo describes info about host
	  */
	 struct __hostInfo
	 {
	 	std::string name;///< original name of the host
	 	stringArr aliases;///< aliases of the host
	 	stringArr addresses;///< addresses of the host
	 };
	 
	/**
	 * @struct __servInfo describes info about service
	 */
	struct __servInfo
	{
	 	std::string name;///< original name of the service
	 	stringArr aliases;///< sevice's aliases
		int port;///< port of service
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
			static stringArr getInterfacesNames(); 
			
			/**
			 * @return info about interface
			 * @param interface is name of interface about what info is needed
			 */
			static __ifInfo getInterfaceInfo(const std::string &interface);
			
			/**
			 * @return info about given host
			 * @param host points to host about what info would be given
			 */
			static __hostInfo getHostInfo(const std::string &host);
			
			/**
			 * @return name of local host
			 */
			static std::string getLocalName();
			
			/**
			 * sets local name
			 * @param host is new  name of the host
			 */
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							setLocalName(const std::string &host);
								
			/**
			 * @return info about service
			 * @param service is service name
			 * @param protocol specifies protocol of service(tcp, udp ..)
			 */
			static __servInfo getServiceInfo(const std::string &service, const std::string &protocol);
								
			/**
			 * @return info about service
			 * @param port is port of service
			 * @param protocol specifies protocol of service(tcp, udp ..)
			 */
			static __servInfo getServiceInfo(int port, const std::string &protocol);
			
	};
	
};

#endif
