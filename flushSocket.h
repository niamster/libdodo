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
		TRANSFER_TYPE_STREAM,///Sequenced, reliable, connection-based byte streams
		TRANSFER_TYPE_DATAGRAM,///Connectionless, unreliable datagrams of fixed maximum length
		TRANSFER_TYPE_RAW,///Raw protocol interface
		TRANSFER_TYPE_PACKET,///Linux specific way of getting packets at the dev level.  For writing rarp and other similar things on the user level
	};
	
	/**
	 * type of domain
	 * as response for more type of protocol and address famaly will increase - i'll add. i don't think that u use appletalk or ipx! as for start - these !
	 */
	enum socketProtoFamilyEnum
	{
		PROTO_FAMILY_IPV4,
		PROTO_FAMILY_IPV6,
		PROTO_FAMILY_PACKET,		
	#ifndef WIN	
		PROTO_FAMILY_UNIX_SOCKET,
	#endif
	};
	
	/**
	 * Options for socket
	 */
	 enum socketOptionsEnum
	 {
	 	SOCKET_KEEP_ALIVE=2,///Keeps  connections  active by enabling the periodic transmission of messages, if this is supported by the protocol.
	 	SOCKET_REUSE_ADDRESS=4,	 	
	 	SOCKET_DONOT_USE_GATEWAY=8, 	
	 	SOCKET_BROADCAST=16,///Permits  sending of broadcast messages, if this is supported by the protocol. 	
	 	SOCKET_OOB_INLINE=32,///out-of-band(marked urgent) data keep inline in recieve operation
	 };
	 /**
	  * send unsent messages in socket queue if close called
	  */
	 enum socketLingerOption
	 { 
	 	SOCKET_GRACEFUL_CLOSE=64,///close returns immediately, but any unsent data is transmitted (after close returns).
	 	SOCKET_HARD_CLOSE=128,///close returns immediately, and any unsent data is discarded.
	 	SOCKET_WAIT_CLOSE=256,///close does not return until all unsent data is transmitted (or the connection is closed by the remote system).
	 };
	/**
	 * class that takes ugly routine with sockets
	 * 
	 */
	class flushSocket : protected flush
	{
		public:
			/**
			 * constructors/destructors
			 * for protocols see /etc/protocols (*nix) or man protocols
			 * some of them:
			 *  ip      0       IP              # internet protocol, pseudo protocol number
			 *	icmp    1       ICMP            # internet control message protocol
			 *	ggp     3       GGP             # gateway-gateway protocol
			 *	st      5       ST              # ST datagram mode
			 *	tcp     6       TCP             # transmission control protocol
			 *	egp     8       EGP             # exterior gateway protocol
			 *	udp     17      UDP             # user datagram protocol
			 *	ipv6    41      IPv6            # IPv6
			 *	idrp    45      IDRP            # Inter-Domain Routing Protocol
			 *	ipv6-icmp 58    IPv6-ICMP       # ICMP for IPv6
			 */
			flushSocket(unsigned long numberOfConn, socketProtoFamilyEnum family, socketTransferTypeEnum type, unsigned int protocol);///for server
			flushSocket(socketProtoFamilyEnum family, socketTransferTypeEnum type, unsigned int protocol);///for client
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
		
			/**
			 * set socket options.
			 */
			void setSockOption(socketOptionsEnum option, bool flag);
			void setLingerSocketOption(socketLingerOption option, int seconds=1);///seconds is used only for SOCKET_WAIT_CLOSE
			 
			void setInBufferSize(int bytes);///accept value to socket; size of socket buffer!
			void setOutBufferSize(int bytes);///accept value to socket; size of socket buffer!
			int getInBufferSize();
			int getOutBufferSize();
						
			void setInTimeout(unsigned long microseconds);///accept value to socket; timeout for operation
			void setOutTimeout(unsigned long microseconds);///accept value to socket; timeout for operation
			unsigned long getInTimeout();
			unsigned long getOutTimeout();
			
			/**
			 * socketOptionsEnum or socketLingerOption
			 */
			bool getSocketOpts(int option);
			
		private:				

			unsigned long inTimeout;///in microseconds
			unsigned long outTimeout;///in microseconds
			 
			int inSocketBuffer;
			int outSocketBuffer;
			 
			virtual void makeSocket(socketProtoFamilyEnum domain, socketTransferTypeEnum type, unsigned int protocol);

			/**
			 * number of connections that can recieve
			 */			 
			long numberOfConn;///default number of connection = 1
			 
			int *connections;///aray with connections.
			int socket;///id of socket
			 
			socketProtoFamilyEnum family;
			socketTransferTypeEnum type;
			unsigned int protocol;
			
			int socketOpts;
	};
};

#endif /*SOCKETPP_H_*/
