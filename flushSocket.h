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
#include "flushDisk.h"

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

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
		TRANSFER_TYPE_RAW,/// you have to send with the data headers or any other info! chose protocol and you'll have access to headers of that proto. If you'll chose RAW - you have to create IP header by yourself!
	};
	
	/**
	 * type of domain
	 * as response for more type of protocol and address famaly will increase - i'll add. i don't think that u use appletalk or ipx! as for start - these !
	 */
	enum socketProtoFamilyEnum
	{
		PROTO_FAMILY_IPV4,
		PROTO_FAMILY_IPV6,		
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
	 	#ifdef SO_REUSEPORT
		 	SOCKET_REUSE_PORT=512,
		#endif
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
	  * info about host
	  */
	 struct __hostInfo
	 {
	 	std::string name;
	 	stringArr aliases;
	 	stringArr addresses;
	 };
	 
	/**
	 * info about service
	 */
	struct __servInfo
	{
	 	std::string name;
	 	stringArr aliases;
		int port;	
	};
	
	class flushSocketExchange;///to make flushSocket and flushSocketExchange mutual friends
	
	/**
	 * class that takes ugly routine with sockets;
	 * exchange of data is flushSocketExchange class' task
	 * this class can establish connections; the below class can send/recieve data!
	 */
	class flushSocket : public flush
	{
		friend class flushSocketExchange;
		
		public:
		
			/**
			 * constructors/destructors
			 * for protocols see /etc/protocols (*nix) or man protocols or /usr/include/netinet/in.h
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
			 * 	raw		255		RAW				# Raw IP packets
			 * 
			 * umay leave protocol = 0(as default) for TRANSFER_TYPE_STREAM or TRANSFER_TYPE_DATAGRAM, but sometimes you should pass it for TRANSFER_TYPE_RAW
			 */
			flushSocket(unsigned long numberOfConn, socketProtoFamilyEnum family, socketTransferTypeEnum type, unsigned int protocol=0);///for server
			flushSocket(socketProtoFamilyEnum family, socketTransferTypeEnum type, unsigned int protocol=0);///for client
			~flushSocket();
			
			/**
			 * connect. for client part
			 * host - ip address; call getHostInfo to get address' for it.
			 * first - net connection
			 * second - local connectioin
			 * if u use unix-socket - it will create it.
			 * u do not have to construct flushSocketExchange!! the function will construct it!
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							connect(const std::string &host, unsigned int port, flushSocketExchange &exchange);
			#ifndef WIN
				#ifndef NO_EX
					virtual void 
				#else
					virtual bool 
				#endif
								connect(const std::string &path, flushSocketExchange &exchange);///if socket is already created - nothin' will be done for creation. if file exists, but not socket - ex will be thrown (or false will be returned)!
			#endif
			
			/**
			 * connect. for server part
			 * host - ip address; call getHostInfo to get address' for it.
			 * first - net connection
			 * second - local connectioin
			 * if u use unix-socket - it will create it. 
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							bindNListen(const std::string &host, unsigned int port);///host - can be '*' -> any address
			#ifndef WIN
				#ifndef NO_EX
					virtual void 
				#else
					virtual bool 
				#endif
								bindNListen(const std::string &path);///if socket is already created - nothin' will be done for creation. if file exists, but not socket - ex will be thrown (or false will be returned)!
			#endif			
					
			/**
			 * get info about given host
			 */
			static __hostInfo getHostInfo(const std::string &host);
			/**
			 * get name of localhost
			 */
			static std::string getLocalName();
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							setLocalName(const std::string &host);
			
			/**
			 * get info about service(port, protocol...)
			 */
			static __servInfo getServiceInfo(const std::string &service, const std::string &protocol);
			static __servInfo getServiceInfo(int port, const std::string &protocol);
			
		private:				

			#ifndef WIN
			
				#ifndef NO_EX
					virtual void 
				#else
					virtual bool 
				#endif
								makeUnixSocket(const std::string &path);
								
			#endif	
			
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							makeSocket(socketProtoFamilyEnum domain, socketTransferTypeEnum type, unsigned int protocol);

			/**
			 * closes main socket for server part(called only in destructor)
			 */
			void _close(); 
			/**
			 * number of connections that can recieve
			 */			 
			long numberOfConn;///number of connection for client = -1
			
			int socket;///id of socket
			
			socketProtoFamilyEnum family;
			socketTransferTypeEnum type;
			unsigned int protocol;
	};
	
	/**
	 * class used for send/recieve data
	 * you may use it's functions only after passing it to connect(accept)
	 * otherwise you'll recieve exeptions about socket(or false) from all of this' class' methods
	 */
	 class flushSocketExchange : public flush
	 {
	 	
	 	friend class flushSocket;
	 	
		private:
		
		public:
		
			flushSocketExchange();
			~flushSocketExchange();
		
								
			/**
			 * send, recieve
			 */
		//	virtual bool send(void *data);
		//	virtual bool recieve(void *data);
			
			/**
			 * set socket options.
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							setSockOption(socketOptionsEnum option, bool flag);
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							setLingerSockOption(socketLingerOption option, int seconds=1);///seconds is used only for SOCKET_WAIT_CLOSE
			
			/**
			 * The maximum buffer size for stream sockets is 262144 bytes
			 */ 
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							setInBufferSize(int bytes);///accept value to socket; size of socket buffer!
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							setOutBufferSize(int bytes);///accept value to socket; size of socket buffer!
			int getInBufferSize();
			int getOutBufferSize();
				
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							setInTimeout(unsigned long microseconds);///accept value to socket; timeout for operation
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							setOutTimeout(unsigned long microseconds);///accept value to socket; timeout for operation
			unsigned long getInTimeout();
			unsigned long getOutTimeout();
			
			/**
			 * socketOptionsEnum or socketLingerOption
			 */
			bool getSocketOpts(int option);
			
			/**
			 * closes this stream
			 */			
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							close();
			
		protected:

			virtual void init(flushSocket *connector, int socket);
					
			int socketOpts;
			
			unsigned long inTimeout;///in microseconds
			unsigned long outTimeout;///in microseconds
			 
			int inSocketBuffer;
			int outSocketBuffer;
			
			int socket;///id of socket	
					
			socketProtoFamilyEnum family;
			socketTransferTypeEnum type;
			unsigned int protocol;
	 };
};

#endif /*SOCKETPP_H_*/
