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
 
#ifndef _FLUSHSOCKET_H_
#define _FLUSHSOCKET_H_

#include "types.h"
#include "directives.h"
#include "flush.h"
#include "flushSocketEx.h"
#include "flushDiskEx.h"
#include "flushDisk.h"

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef WIN
	#include <winsock.h>
#else
	#include <sys/un.h>
	#include <sys/socket.h>
#endif

namespace dodo
{
	
	#ifndef WIN
		#define UNIX_SOCKET_PERM (OWNER_READ_ACCESS|OWNER_WRITE_ACCESS)
	#endif
	/**
	 * type of socket to use
	 */
	enum socketTransferTypeEnum
	{
		STREAM,///Sequenced, reliable, connection-based byte streams
		DATAGRAM,///Connectionless, unreliable datagrams of fixed maximum length
		RAW,///Raw protocol interface
		PACKET,///Linux specific way of getting packets at the dev level.  For writing rarp and other similar things on the user level
	};
	
	/**
	 * type of domain
	 * as response for more type of protocol and address famaly will increase - i'll add. i don't think that u use appletalk or ipx! as for start - these !
	 */
	enum socketDomainEnum
	{
		IPV4,
		IPV6,		
	#ifndef WIN	
		UNIX_SOCKET,
	#endif
	};
	/**
	 * class that takes ugly routine with sockets
	 * 
	 */
	class flushSocket : public flush
	{
		public:
			/**
			 * constructors/destructors
			 * for protocols see /etc/protocols (*nix) or man protocols
			 * some of them:
			 *  ip      0       IP              # internet protocol, pseudo protocol number
			 *	icmp    1       ICMP            # internet control message protocol
			 *	igmp    2       IGMP            # Internet Group Management
			 *	ggp     3       GGP             # gateway-gateway protocol
			 *	ipencap 4       IP-ENCAP        # IP encapsulated in IP (officially ``IP'')
			 *	st      5       ST              # ST datagram mode
			 *	tcp     6       TCP             # transmission control protocol
			 *	egp     8       EGP             # exterior gateway protocol
			 *	pup     12      PUP             # PARC universal packet protocol
			 *	udp     17      UDP             # user datagram protocol
			 *	hmp     20      HMP             # host monitoring protocol
			 *	xns-idp 22      XNS-IDP         # Xerox NS IDP
			 *	rdp     27      RDP             # "reliable datagram" protocol
			 *	iso-tp4 29      ISO-TP4         # ISO Transport Protocol class 4
			 *	xtp     36      XTP             # Xpress Tranfer Protocol
			 *	ddp     37      DDP             # Datagram Delivery Protocol
			 *	idpr-cmtp 38    IDPR-CMTP       # IDPR Control Message Transport
			 *	ipv6    41      IPv6            # IPv6
			 *	ipv6-route 43   IPv6-Route      # Routing Header for IPv6
			 *	ipv6-frag 44    IPv6-Frag       # Fragment Header for IPv6
			 *	idrp    45      IDRP            # Inter-Domain Routing Protocol
			 *	rsvp    46      RSVP            # Reservation Protocol
			 *	gre     47      GRE             # General Routing Encapsulation
			 *	esp     50      ESP             # Encap Security Payload for IPv6
			 *	ah      51      AH              # Authentication Header for IPv6
			 *	skip    57      SKIP            # SKIP
			 *	ipv6-icmp 58    IPv6-ICMP       # ICMP for IPv6
			 *	ipv6-nonxt 59   IPv6-NoNxt      # No Next Header for IPv6
			 *	ipv6-opts 60    IPv6-Opts       # Destination Options for IPv6
			 *	rspf    73      RSPF            # Radio Shortest Path First.
			 *	vmtp    81      VMTP            # Versatile Message Transport
			 *	ospf    89      OSPFIGP         # Open Shortest Path First IGP
			 *	ipip    94      IPIP            # IP-within-IP Encapsulation Protocol
			 *	encap   98      ENCAP           # Yet Another IP encapsulation
			 *	pim     103     PIM             # Protocol Independent Multicast
			 * 
			 */
			flushSocket(unsigned long numberOfConn, socketDomainEnum domain, socketTransferTypeEnum type, unsigned int protocol);///for server
			flushSocket(socketDomainEnum domain, socketTransferTypeEnum type, unsigned int protocol);///for client
			~flushSocket();
			
			/**
			 * connect. 
			 * first - net connection
			 * second - local connectioin
			 * if u use unix-socket - it will create it. 
			 */
			virtual bool connect(const std::string &host, unsigned int port);
			#ifndef WIN
				virtual bool connect(const std::string &path);///if socket is already created - nothin' will be done for creation. if file exists, but not socket - ex will be thrown (or false will be returned)!
			#endif
			
			/**
			 * send, recieve
			 */
		//	virtual bool send(void *data);
		//	virtual bool recieve(void *data);
			
		//	virtual bool listen(int &id);
			
		//	virtual socketDomainEnum getDomain();
		//	virtual socketTransferTypeEnum getType();
		//	virtual int getProtocol();
		protected:				

			/**
			 * number of connections that can recieve
			 */
			 
			 virtual void makeSocket(socketDomainEnum domain, socketTransferTypeEnum type, unsigned int protocol);
			 
			 long numberOfConn;///default number of connection = 1
			 
			 int *connections;///aray with connections.
			 int socket;///id of socket
			 
			socketDomainEnum domain;
			socketTransferTypeEnum type;
			unsigned int protocol;
	};
};

#endif /*SOCKETPP_H_*/
